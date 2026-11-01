#include "MythSaveSubsystem.h"
#include "MythSaveGame.h"
#include "MythSaveStorage.h"
#include "Async/Async.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Package.h"

void UMythSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ReloadFromDisk();
}

void UMythSaveSubsystem::Deinitialize()
{
    // Worker owns its serialized byte array and checks a weak pointer before callback.
    Super::Deinitialize();
}

void UMythSaveSubsystem::MigrateLoadedSave()
{
    if (!CurrentSave) return;
    if (CurrentSave->SchemaVersion <= 1)
    {
        FMythCampaignSnapshot& Data = CurrentSave->Campaign;
        Data.ChapterId = CurrentSave->ChapterId;
        Data.bHasPlayerState = CurrentSave->bHasPlayerState;
        Data.PlayerTransform = CurrentSave->PlayerTransform;
        Data.PlayerHealth = CurrentSave->PlayerHealth;
        Data.DefeatedBossIds = CurrentSave->DefeatedBossIds;
        CurrentSave->SchemaVersion = 2;
        bDirty = true;
    }
}

bool UMythSaveSubsystem::ReloadFromDisk()
{
    if (bSaving) return false; // Do not race an active disk write.
    const FMythSaveReadResult Read = FMythSaveStorage::ReadBest(FMythSaveStorage::DefaultBasePath());
    if (Read.Status == EMythSaveReadStatus::Corrupt)
    {
        UE_LOG(LogTemp, Error, TEXT("MythARPG save is corrupt; keeping existing files for recovery."));
        return false;
    }
    UMythSaveGame* Candidate = nullptr;
    if (Read.Status == EMythSaveReadStatus::Missing)
        Candidate = Cast<UMythSaveGame>(UGameplayStatics::CreateSaveGameObject(UMythSaveGame::StaticClass()));
    else
        Candidate = Cast<UMythSaveGame>(UGameplayStatics::LoadGameFromMemory(Read.Bytes));
    if (!Candidate) return false;
    if (Candidate->SchemaVersion > 2)
    {
        UE_LOG(LogTemp, Error, TEXT("MythARPG: unsupported future save version %d"), Candidate->SchemaVersion);
        return false;
    }
    CurrentSave = Candidate;
    bDirty = false;
    MutationRevision = 0;
    MigrateLoadedSave();
    if (Read.Status == EMythSaveReadStatus::RecoveredOlderGeneration)
        UE_LOG(LogTemp, Warning, TEXT("MythARPG recovered a previous valid save generation."));
    // Legacy slots migrate into new generation files at next safe save boundary.
    if (Read.Status != EMythSaveReadStatus::Missing && Read.ActiveIndex == INDEX_NONE)
        bDirty = true;
    return true;
}

FMythCampaignSnapshot UMythSaveSubsystem::GetSnapshot() const
{
    return CurrentSave ? CurrentSave->Campaign : FMythCampaignSnapshot();
}

bool UMythSaveSubsystem::ValidateSnapshot(const FMythCampaignSnapshot& S) const
{
    if (S.PlayerLevel < 1 || S.Experience < 0 || S.SkillPoints < 0 ||
        !FMath::IsFinite(S.PlayerHealth) || S.PlayerHealth < 0.f || S.PlayerTransform.ContainsNaN()) return false;
    TSet<FGuid> UniqueInstances;
    for (const FMythItemStack& Item : S.Inventory)
    {
        if (Item.ItemId.IsNone() || Item.Count < 1 || Item.UpgradeLevel < 0) return false;
        if (Item.InstanceId.IsValid())
        {
            if (Item.Count!=1 || UniqueInstances.Contains(Item.InstanceId)) return false;
            UniqueInstances.Add(Item.InstanceId);
        }
    }
    for (const auto& Slot : S.EquippedSlots)
        if (Slot.Key.IsNone() || !UniqueInstances.Contains(Slot.Value))return false;
    for (const auto& Money : S.Wallet)
        if (Money.Key.IsNone() || Money.Value < 0) return false;
    TSet<FName> QuestIds;
    for (const FMythQuestRecord& Quest : S.Quests)
    {
        if (Quest.QuestId.IsNone() || QuestIds.Contains(Quest.QuestId))return false;
        QuestIds.Add(Quest.QuestId);
    }
    return true;
}

bool UMythSaveSubsystem::SetSnapshot(const FMythCampaignSnapshot& NewSnapshot,bool bFlushNow)
{
    if (!CurrentSave || (bFlushNow && bSaving) || !ValidateSnapshot(NewSnapshot)) return false;
    CurrentSave->Campaign = NewSnapshot;
    ++MutationRevision;
    bDirty = true;
    if (bSaving) bQueuedSave = true;
    return !bFlushNow || FlushToDisk();
}

bool UMythSaveSubsystem::SerializeSnapshot(TArray<uint8>& OutBytes) const
{
    // UObject serialization belongs to the game thread; workers do file I/O only.
    return CurrentSave && UGameplayStatics::SaveGameToMemory(CurrentSave, OutBytes);
}

bool UMythSaveSubsystem::FlushToDisk()
{
    if (!CurrentSave || bSaving) return false;
    if (!bDirty) return true;
    TArray<uint8> Bytes;
    const bool bSuccess = SerializeSnapshot(Bytes) &&
        FMythSaveStorage::WriteNext(FMythSaveStorage::DefaultBasePath(), Bytes);
    if (bSuccess) bDirty = false;
    OnSaveFinished.Broadcast(bSuccess);
    return bSuccess;
}

bool UMythSaveSubsystem::RequestAsyncSave()
{
    if (!CurrentSave) return false;
    if (bSaving) { bQueuedSave = true; return true; }
    if (!bDirty) return true;
    TArray<uint8> Bytes;
    if (!SerializeSnapshot(Bytes)) return false;
    bSaving = true;
    InFlightRevision = MutationRevision;
    const FString Path = FMythSaveStorage::DefaultBasePath();
    const TWeakObjectPtr<UMythSaveSubsystem> WeakThis(this);
    Async(EAsyncExecution::ThreadPool, [WeakThis, Path, Bytes = MoveTemp(Bytes)]() mutable
    {
        const bool bOK = FMythSaveStorage::WriteNext(Path, Bytes);
        AsyncTask(ENamedThreads::GameThread, [WeakThis, bOK]()
        {
            if (UMythSaveSubsystem* Sub = WeakThis.Get()) Sub->OnAsyncSaveCompleted(bOK);
        });
    });
    return true;
}

void UMythSaveSubsystem::OnAsyncSaveCompleted(bool bSuccess)
{
    bSaving = false;
    if (bSuccess && InFlightRevision == MutationRevision) bDirty = false;
    const bool bRetry = bQueuedSave || (bSuccess && bDirty);
    bQueuedSave = false;
    OnSaveFinished.Broadcast(bSuccess);
    if (bRetry && bDirty) RequestAsyncSave();
}

bool UMythSaveSubsystem::SavePlayerState(const FTransform& Transform,float Health)
{
    FMythCampaignSnapshot S = GetSnapshot();
    S.PlayerTransform=Transform;
    S.PlayerHealth=FMath::Max(0.f,Health);
    S.bHasPlayerState=true;
    return SetSnapshot(S,true);
}

bool UMythSaveSubsystem::RecordBossDefeat(FName BossId)
{
    if (BossId.IsNone()) return false;
    FMythCampaignSnapshot S = GetSnapshot();
    if (S.DefeatedBossIds.Contains(BossId)) return true;
    S.DefeatedBossIds.Add(BossId);
    return SetSnapshot(S);
}

bool UMythSaveSubsystem::IsBossDefeated(FName BossId) const
{
    return CurrentSave && CurrentSave->Campaign.DefeatedBossIds.Contains(BossId);
}

bool UMythSaveSubsystem::GetSavedPlayerState(FTransform& OutTransform,float& OutHealth) const
{
    if (!CurrentSave || !CurrentSave->Campaign.bHasPlayerState) return false;
    OutTransform=CurrentSave->Campaign.PlayerTransform;
    OutHealth=CurrentSave->Campaign.PlayerHealth;
    return true;
}
