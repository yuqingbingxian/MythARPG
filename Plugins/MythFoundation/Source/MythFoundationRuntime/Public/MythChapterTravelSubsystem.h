#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythCampaignTypes.h"
#include "Engine/StreamableManager.h"
#include "MythChapterTravelSubsystem.generated.h"

UENUM(BlueprintType)
enum class EMythTravelPhase : uint8
{
    Idle, Resolving, Saving, OpeningMap, Arrived, Failed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMythTravelStateChanged, EMythTravelPhase, Phase, FText, Detail);

/**
 * Non-content-specific travel coordinator for packaged chapter maps.
 * Prefetches the map reference, persists a checkpoint-safe snapshot,
 * travels, and confirms arrival through UE's map-loaded callback.
 * This is hard travel (OpenLevel), not intra-world streaming.
 */
UCLASS()
class MYTHFOUNDATIONRUNTIME_API UMythChapterTravelSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UPROPERTY(BlueprintAssignable, Category="Chapter|Travel") FMythTravelStateChanged OnStateChanged;
    UFUNCTION(BlueprintCallable, Category="Chapter|Travel") bool RequestChapterTravel(FName ChapterId, FName CheckpointId);
    /** Explicit escape hatch if map travel cannot complete (e.g. invalid packaged map). */
    UFUNCTION(BlueprintCallable, Category="Chapter|Travel") void CancelPendingTravel();
    UFUNCTION(BlueprintPure, Category="Chapter|Travel") EMythTravelPhase GetPhase() const { return Phase; }
    UFUNCTION(BlueprintPure, Category="Chapter|Travel") FName GetPendingChapter() const { return PendingChapterId; }

private:
    UPROPERTY(Transient) EMythTravelPhase Phase = EMythTravelPhase::Idle;
    UPROPERTY(Transient) FName PendingChapterId = NAME_None;
    UPROPERTY(Transient) FName PendingCheckpointId = NAME_None;
    UPROPERTY(Transient) FString PendingMapPackage;
    FMythCampaignSnapshot BeforeTravel;
    TSharedPtr<FStreamableHandle> MapHandle;
    FDelegateHandle OnMapLoadedHandle;
    bool bCommittedBeforeTravel = false;

    void SetPhase(EMythTravelPhase NewPhase, const FText& Message);
    void OnMapReady();
    void OnMapLoaded(UWorld* LoadedWorld);
    void FailAndRollback(const FText& Reason);
    void ClearPending();
};
