#include "MythChapterTravelSubsystem.h"
#include "MythChapterSubsystem.h"
#include "MythChapterDefinition.h"
#include "MythSaveSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/CoreUObjectDelegates.h"
#include "Misc/PackageName.h"

void UMythChapterTravelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UMythSaveSubsystem>();
    Collection.InitializeDependency<UMythChapterSubsystem>();
    Super::Initialize(Collection);
    OnMapLoadedHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMythChapterTravelSubsystem::OnMapLoaded);
}

void UMythChapterTravelSubsystem::Deinitialize()
{
    FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(OnMapLoadedHandle);
    CancelPendingTravel();
    Super::Deinitialize();
}

void UMythChapterTravelSubsystem::SetPhase(EMythTravelPhase NewPhase, const FText& Message)
{
    Phase = NewPhase;
    OnStateChanged.Broadcast(Phase, Message);
}

bool UMythChapterTravelSubsystem::RequestChapterTravel(FName ChapterId, FName CheckpointId)
{
    if (Phase == EMythTravelPhase::Resolving || Phase == EMythTravelPhase::Saving || Phase == EMythTravelPhase::OpeningMap)
        return false;
    UGameInstance* GI = GetGameInstance();
    if (!GI || !GI->GetWorld()) return false;
    UMythChapterSubsystem* Chapter = GI->GetSubsystem<UMythChapterSubsystem>();
    UMythSaveSubsystem* Save = GI->GetSubsystem<UMythSaveSubsystem>();
    if (!Chapter || !Save || !Chapter->IsChapterUnlocked(ChapterId) || Save->IsSaveInProgress()) return false;
    const UMythChapterDefinition* Def = Chapter->FindChapter(ChapterId);
    if (!Def || Def->EntryMap.IsNull()) return false;

    BeforeTravel = Save->GetSnapshot();
    PendingChapterId = ChapterId;
    PendingCheckpointId = CheckpointId.IsNone() ? Def->EntryCheckpointId : CheckpointId;
    PendingMapPackage = Def->EntryMap.ToSoftObjectPath().GetLongPackageName();
    if (PendingMapPackage.IsEmpty()) { ClearPending(); return false; }
    bCommittedBeforeTravel = false;
    SetPhase(EMythTravelPhase::Resolving, FText::FromString(TEXT("Loading chapter map reference")));
    MapHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        Def->EntryMap.ToSoftObjectPath(),
        FStreamableDelegate::CreateUObject(this, &UMythChapterTravelSubsystem::OnMapReady));
    if (!MapHandle)
    {
        FailAndRollback(FText::FromString(TEXT("Unable to request chapter asset")));
        return false;
    }
    return true;
}

void UMythChapterTravelSubsystem::OnMapReady()
{
    UGameInstance* GI = GetGameInstance();
    if (Phase != EMythTravelPhase::Resolving || !GI || !GI->GetWorld()) return;
    UMythChapterSubsystem* Chapter = GI->GetSubsystem<UMythChapterSubsystem>();
    UMythSaveSubsystem* Save = GI->GetSubsystem<UMythSaveSubsystem>();
    if (!Chapter || !Save || !MapHandle || !MapHandle->HasLoadCompleted() ||
        !MapHandle->GetLoadedAsset())
    {
        FailAndRollback(FText::FromString(TEXT("Chapter map asset failed to load")));
        return;
    }

    SetPhase(EMythTravelPhase::Saving, FText::FromString(TEXT("Committing chapter transition")));
    // SelectChapter flushes the checkpoint and clears the previous level's transform.
    // Transition is committed BEFORE OpenLevel so BeginPlay sees the right chapter.
    FMythCampaignSnapshot TravelSnapshot=Save->GetSnapshot();
    TravelSnapshot.TravelRecovery.bPending=true;
    TravelSnapshot.TravelRecovery.PreviousChapterId=BeforeTravel.ChapterId;
    TravelSnapshot.TravelRecovery.PreviousCheckpointId=BeforeTravel.CheckpointId;
    TravelSnapshot.TravelRecovery.PreviousPlayerTransform=BeforeTravel.PlayerTransform;
    TravelSnapshot.TravelRecovery.PreviousHealth=BeforeTravel.PlayerHealth;
    TravelSnapshot.TravelRecovery.bPreviousPlayerState=BeforeTravel.bHasPlayerState;
    const bool bPrepared=Save->SetSnapshot(TravelSnapshot,true);
    if(bPrepared)bCommittedBeforeTravel=true;
    if (!bPrepared || !Chapter->SelectChapter(PendingChapterId, PendingCheckpointId))
    {
        FailAndRollback(FText::FromString(TEXT("Unable to persist chapter transition")));
        return;
    }
    SetPhase(EMythTravelPhase::OpeningMap, FText::FromString(TEXT("Opening chapter map")));
    UGameplayStatics::OpenLevel(GI->GetWorld(), FName(*PendingMapPackage), true);
}

void UMythChapterTravelSubsystem::OnMapLoaded(UWorld* LoadedWorld)
{
    if (Phase != EMythTravelPhase::OpeningMap || !LoadedWorld) return;
    // PIE may prepend UEDPIE_n_ to the short name. Check suffix without guessing a travel duration.
    const FString Actual = FPackageName::GetShortName(LoadedWorld->GetOutermost()->GetName());
    const FString Expected = FPackageName::GetShortName(PendingMapPackage);
    if (Actual == Expected || Actual.EndsWith(TEXT("_") + Expected))
    {
        if(UGameInstance* GI=GetGameInstance())
        {
            if(UMythSaveSubsystem* Save=GI->GetSubsystem<UMythSaveSubsystem>())
            {
                FMythCampaignSnapshot NewSnapshot=Save->GetSnapshot();
                NewSnapshot.TravelRecovery=FMythTravelRecovery();
                if(!Save->SetSnapshot(NewSnapshot,true))
                {FailAndRollback(FText::FromString(TEXT("Unable to confirm destination checkpoint")));return;}
            }
        }
        SetPhase(EMythTravelPhase::Arrived, FText::FromString(TEXT("Chapter map ready")));
        ClearPending();
        return;
    }
    FailAndRollback(FText::FromString(TEXT("Unexpected map loaded during chapter transition")));
}

void UMythChapterTravelSubsystem::FailAndRollback(const FText& Reason)
{
    if (bCommittedBeforeTravel)
    {
        if (UGameInstance* GI = GetGameInstance())
            if (UMythSaveSubsystem* Save = GI->GetSubsystem<UMythSaveSubsystem>())
                if (!Save->SetSnapshot(BeforeTravel, true))
                    UE_LOG(LogTemp, Error, TEXT("MythARPG travel rollback failed: persist the previous snapshot manually."));
    }
    ClearPending();
    SetPhase(EMythTravelPhase::Failed, Reason);
}

void UMythChapterTravelSubsystem::ClearPending()
{
    if (MapHandle) MapHandle->ReleaseHandle();
    MapHandle.Reset();
    PendingChapterId = NAME_None;
    PendingCheckpointId = NAME_None;
    PendingMapPackage.Reset();
    bCommittedBeforeTravel = false;
}

void UMythChapterTravelSubsystem::CancelPendingTravel()
{
    if (Phase == EMythTravelPhase::Resolving || Phase == EMythTravelPhase::Saving || Phase == EMythTravelPhase::OpeningMap)
        FailAndRollback(FText::FromString(TEXT("Chapter travel cancelled")));
}
