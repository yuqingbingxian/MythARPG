#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataAsset.h"
#include "MythProgressionSubsystem.generated.h"

UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythProgressionRules : public UDataAsset
{
    GENERATED_BODY()
public:
    // XP required to advance from level N to N+1, zero-based array index is N-1.
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<int32> ExperiencePerLevel;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin="0")) int32 SkillPointsPerLevel=1;
};
UCLASS()
class UMythSkillTreeDefinition;
class MYTHSYSTEMSRUNTIME_API UMythProgressionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) void RegisterRules(UMythProgressionRules* InRules);
    UFUNCTION(BlueprintCallable) void RegisterSkillTree(UMythSkillTreeDefinition* Tree);
    UFUNCTION(BlueprintCallable) bool AwardExperience(int32 Amount);
    UFUNCTION(BlueprintCallable) bool UnlockSkill(FName SkillId,int32 Cost=1);
    UFUNCTION(BlueprintPure) bool IsSkillUnlocked(FName SkillId) const;
private:
    UPROPERTY(Transient) TObjectPtr<UMythProgressionRules> Rules;
    UPROPERTY(Transient) TObjectPtr<UMythSkillTreeDefinition> SkillTree;
};
