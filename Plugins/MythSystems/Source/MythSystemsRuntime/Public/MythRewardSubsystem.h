#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythCampaignTypes.h"
#include "MythRewardSubsystem.generated.h"
class UMythRewardDefinition;
/** Applies all grants to a transaction copy and only persists after successful validation. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythRewardSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) bool GrantReward(const UMythRewardDefinition* Definition);
    bool TryGrantToSnapshot(FMythCampaignSnapshot& S,const UMythRewardDefinition* Definition)const;
};
