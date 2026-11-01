#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MythDodgeAbility.generated.h"
UCLASS()
class MYTHGAMEPLAYRUNTIME_API UMythDodgeAbility : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UMythDodgeAbility();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicateEndAbility,bool bWasCancelled) override;
    void CompleteFromAction();
};
