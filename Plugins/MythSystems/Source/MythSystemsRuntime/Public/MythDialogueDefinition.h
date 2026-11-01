#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythDialogueDefinition.generated.h"
class UMythNarrativeCommandDefinition;
USTRUCT(BlueprintType)
struct FMythDialogueChoice
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FText Label;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName NextNodeId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName RequiredWorldFlag=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName SetWorldFlag=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TSoftObjectPtr<UMythNarrativeCommandDefinition> ActionSequence;
};
USTRUCT(BlueprintType)
struct FMythDialogueNode
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName NodeId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName SpeakerId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FText Text;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TSoftObjectPtr<USoundBase> Voice;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FMythDialogueChoice> Choices;
};
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythDialogueDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName DialogueId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName StartNodeId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythDialogueNode> Nodes;
    UFUNCTION(BlueprintPure) bool HasNode(FName NodeId) const;
    const FMythDialogueNode* FindNode(FName NodeId) const;
};
