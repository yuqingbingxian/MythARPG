#pragma once
#include "CoreMinimal.h"
#include "MythCampaignTypes.generated.h"

// All identifiers are authored stable IDs. Never persist actor pointers or generated runtime names.
USTRUCT(BlueprintType)
struct MYTHFOUNDATIONRUNTIME_API FMythItemStack
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName ItemId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Count = 0;
    // Blank for stackable items; persistent for unique equipment instances.
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FGuid InstanceId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 UpgradeLevel = 0;
};

USTRUCT(BlueprintType)
struct MYTHFOUNDATIONRUNTIME_API FMythQuestRecord
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName QuestId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName StageId = NAME_None;
    // 0=Locked, 1=Active, 2=Completed, 3=Failed (enum lives in Quest module).
    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 Status = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName,int32> ObjectiveCounts;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<FName> RewardReceiptIds;
};


/** Persisted outbox: at-least-once events, delivered after the state commit. */
USTRUCT(BlueprintType)
struct MYTHFOUNDATIONRUNTIME_API FMythQueuedSignal
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName ReceiptId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName EventName=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName TargetId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName ChapterId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 Count=1;
};

/** Crash-recovery breadcrumb while OpenLevel is in progress. */
USTRUCT(BlueprintType)
struct MYTHFOUNDATIONRUNTIME_API FMythTravelRecovery
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool bPending=false;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName PreviousChapterId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FName PreviousCheckpointId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) FTransform PreviousPlayerTransform=FTransform::Identity;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) float PreviousHealth=100.f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite) bool bPreviousPlayerState=false;
};

USTRUCT(BlueprintType)
struct MYTHFOUNDATIONRUNTIME_API FMythCampaignSnapshot
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName ChapterId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName CheckpointId = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasPlayerState = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FTransform PlayerTransform = FTransform::Identity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float PlayerHealth = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 PlayerLevel = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Experience = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SkillPoints = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName,int64> Wallet;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FMythItemStack> Inventory;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName,FGuid> EquippedSlots;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<FName> UnlockedSkills;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FMythQuestRecord> Quests;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FName,int32> WorldValues;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<FName> DefeatedBossIds;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<FName> UnlockedChapterIds;
    // These record one-time application of quest rewards, pickups, and narrative actions.
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSet<FName> AppliedTransactionIds;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FMythQueuedSignal> PendingSignals;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FMythTravelRecovery TravelRecovery;
};
