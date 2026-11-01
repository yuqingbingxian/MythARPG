#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythQuestDefinition.generated.h"

// EventName examples: EnemyDefeated, ItemCollected, DialogueCompleted, BossDefeated.
USTRUCT(BlueprintType)
struct FMythQuestObjective
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName ObjectiveId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName EventName=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName TargetId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="1")) int32 RequiredCount=1;
};
USTRUCT(BlueprintType)
struct FMythQuestStage
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName StageId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FMythQuestObjective> Objectives;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName NextStageId=NAME_None; // None = quest complete.
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName WorldFlagOnCompletion=NAME_None;
};
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythQuestDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName QuestId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName ChapterId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName InitialStageId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythQuestStage> Stages;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FName> RequiredWorldFlags;
    virtual FPrimaryAssetId GetPrimaryAssetId()const override
    {return FPrimaryAssetId(TEXT("Quest"),QuestId.IsNone()?GetFName():QuestId);}
    const FMythQuestStage* FindStage(FName StageId) const;
    bool ValidateDefinition(FString& Error) const;
};
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythQuestCatalog : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<TSoftObjectPtr<UMythQuestDefinition>> Quests;
    UFUNCTION(BlueprintCallable) UMythQuestDefinition* FindQuest(FName QuestId) const;
};
