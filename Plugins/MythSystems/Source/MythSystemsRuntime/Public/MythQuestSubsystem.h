#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythQuestSubsystem.generated.h"
class UMythQuestCatalog;
struct FMythGameplaySignal;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMythQuestTransition,FName,QuestId,FName,StageId);

/** Generic ordered stage executor; stage completion and world flag are one snapshot mutation. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythQuestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    UPROPERTY(BlueprintAssignable) FMythQuestTransition OnQuestAdvanced;
    UPROPERTY(BlueprintAssignable) FMythQuestTransition OnQuestCompleted;
    UFUNCTION(BlueprintCallable) void RegisterCatalog(UMythQuestCatalog* Catalog);
    UFUNCTION(BlueprintCallable) bool StartQuest(FName QuestId);
    UFUNCTION(BlueprintCallable) bool ReportGameplayEvent(FName EventName,FName TargetId,int32 Amount=1,FName StableReceiptId=NAME_None);
    UFUNCTION(BlueprintPure) FName GetCurrentStage(FName QuestId) const;
private:
    UPROPERTY(Transient) TObjectPtr<UMythQuestCatalog> QuestCatalog;
    void HandleSignal(const FMythGameplaySignal& Signal);
};
