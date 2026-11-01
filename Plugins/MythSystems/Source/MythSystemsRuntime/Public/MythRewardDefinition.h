#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythCampaignTypes.h"
#include "MythRewardDefinition.generated.h"
USTRUCT(BlueprintType)
struct FMythCurrencyReward
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName CurrencyId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="0")) int64 Amount=0;
};
/** One grantable bundle. RewardId must be globally unique across quests, pickups and bosses. */
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythRewardDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName RewardId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythItemStack> Items;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythCurrencyReward> Currency;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin="0")) int32 Experience=0;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin="0")) int32 SkillPoints=0;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FName> SetWorldFlags;
};
