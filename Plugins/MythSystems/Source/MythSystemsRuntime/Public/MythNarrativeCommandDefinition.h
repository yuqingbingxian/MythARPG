#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythNarrativeCommandDefinition.generated.h"
class UMythRewardDefinition;
UENUM(BlueprintType)
enum class EMythNarrativeOpcode : uint8
{
    SetWorldValue, AddWorldValue, AddCurrency, UnlockChapter, GrantReward, EmitGameplaySignal
};
USTRUCT(BlueprintType)
struct MYTHSYSTEMSRUNTIME_API FMythNarrativeCommand
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) EMythNarrativeOpcode Opcode=EMythNarrativeOpcode::SetWorldValue;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName TargetId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) int32 IntValue=1;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TSoftObjectPtr<UMythRewardDefinition> Reward;
};
/** Stable receipt prevents reapplying reward or one-time world transitions. */
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythNarrativeCommandDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName ReceiptId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName RequiredWorldFlag=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythNarrativeCommand> Commands;
    UFUNCTION(BlueprintPure) bool Validate(FString& OutError)const;
};
