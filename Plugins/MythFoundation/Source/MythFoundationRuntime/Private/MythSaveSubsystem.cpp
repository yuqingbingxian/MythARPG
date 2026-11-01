#include "MythSaveSubsystem.h"
#include "MythSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Package.h"

void UMythSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    ReloadFromDisk();
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
    if (bSaving) return false; // Never replace a snapshot during an active write.
    CurrentSave = Cast<UMythSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName,0));
    if (!CurrentSave)
    {
        CurrentSave = Cast<UMythSaveGame>(UGameplayStatics::CreateSaveGameObject(UMythSaveGame::StaticClass()));
        if (CurrentSave) CurrentSave->Campaign.ChapterId = TEXT("CH01");
    }
    if (!CurrentSave) return false;
    if (CurrentSave->SchemaVersion > 2)
    {
        UE_LOG(LogTemp,Error,TEXT("MythARPG: unsupported future save version %d"),CurrentSave->SchemaVersion);
        CurrentSave = nullptr;
        return false;
    }
    bDirty = false;
    MutationRevision = 0;
    MigrateLoadedSave();
    return true;
}

FMythCampaignSnapshot UMythSaveSubsystem::GetSnapshot() const
{
    return CurrentSave ? CurrentSave->Campaign : FMythCampaignSnapshot();
}

bool UMythSaveSubsystem::ValidateSnapshot(const FMythCampaignSnapshot& S) const
{
    if (S.PlayerLevel < 1 || S.Experience < 0 || S.SkillPoints < 0 || !FMath::IsFinite(S.PlayerHealth) || S.PlayerHealth < 0.f) return false;
    for (const FMythItemStack& Item : S.Inventory)
        if (Item.ItemId.IsNone() || Item.Count < 1 || Item.UpgradeLevel < 0) return false;
    for (const auto& Money : S.Wallet)
        if (Money.Key.IsNone() || Money.Value < 0) return false;
    return true;
}

bool UMythSaveSubsystem::SetSnapshot(const FMythCampaignSnapshot& NewSnapshot,bool bFlushNow)
{
    if (!CurrentSave || !ValidateSnapshot(NewSnapshot)) return false;
    CurrentSave->Campaign = NewSnapshot;
    ++MutationRevision;
    bDirty = true;
    if (bSaving) bQueuedSave = true;
    return !bFlushNow || FlushToDisk();
}

bool UMythSaveSubsystem::FlushToDisk()
{
    if (!CurrentSave || bSaving) return false;
    if (!bDirty) return true;
    const bool bSuccess = UGameplayStatics::SaveGameToSlot(CurrentSave,SlotName,0);
    if (bSuccess) bDirty = false;
    OnSaveFinished.Broadcast(bSuccess);
    return bSuccess;
}

bool UMythSaveSubsystem::RequestAsyncSave()
{
    if (!CurrentSave) return false;
    if (bSaving) { bQueuedSave = true; return true; }
    if (!bDirty) return true;
    PendingSave = DuplicateObject<UMythSaveGame>(CurrentSave,GetTransientPackage());
    if (!PendingSave) return false;
    bSaving = true;
    InFlightRevision = MutationRevision;
    const FAsyncSaveGameToSlotDelegate Done = FAsyncSaveGameToSlotDelegate::CreateUObject(this,&UMythSaveSubsystem::OnAsyncSaveCompleted);
    UGameplayStatics::AsyncSaveGameToSlot(PendingSave,SlotName,0,Done);
    return true;
}

void UMythSaveSubsystem::OnAsyncSaveCompleted(const FString& Slot,int32 UserIndex,bool bSuccess)
{
    bSaving = false;
    PendingSave = nullptr;
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
    return SetSnapshot(S,true);
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
