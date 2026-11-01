#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MythMeleeAbility.generated.h"

/** GAS is the activation/cancellation owner; MythActionComponent owns node-level combat timing. */
UCLASS()
class MYTHGAMEPLAYRUNTIME_API UMythMeleeAbility : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UMythMeleeAbility();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicateEndAbility,bool bWasCancelled) override;
    void CompleteFromAction();
    void CancelFromAction();
};
