#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythSkillTreeDefinition.generated.h"
USTRUCT(BlueprintType)
struct FMythSkillNode
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName SkillId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FName> Prerequisites;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="0")) int32 SkillPointCost=1;
};
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythSkillTreeDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythSkillNode> Nodes;
    const FMythSkillNode* FindSkill(FName SkillId)const;
    bool ValidateTree(FString& OutError) const;
};
