#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythNarrativeSubsystem.generated.h"
class UMythDialogueDefinition;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMythDialogueEvent,FName,DialogueId,FName,NodeId);

/** Dialogue runtime owns cursor and choice validation; UI merely displays nodes. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythNarrativeSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable) FMythDialogueEvent OnNodeEntered;
    UPROPERTY(BlueprintAssignable) FMythDialogueEvent OnDialogueFinished;
    UFUNCTION(BlueprintCallable) bool StartDialogue(UMythDialogueDefinition* Definition);
    UFUNCTION(BlueprintCallable) bool Choose(int32 ChoiceIndex);
    UFUNCTION(BlueprintCallable) void StopDialogue();
    UFUNCTION(BlueprintPure) FName CurrentDialogueId() const;
    UFUNCTION(BlueprintPure) FName CurrentNodeId() const {return ActiveNodeId;}
    UFUNCTION(BlueprintPure) bool CanChoose(int32 Index)const;
private:
    UPROPERTY(Transient) TObjectPtr<UMythDialogueDefinition> ActiveDialogue;
    FName ActiveNodeId=NAME_None;
};
