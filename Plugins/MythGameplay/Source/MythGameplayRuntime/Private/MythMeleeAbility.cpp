#include "MythMeleeAbility.h"
#include "MythCombatCharacter.h"
#include "MythActionComponent.h"
UMythMeleeAbility::UMythMeleeAbility()
{
    InstancingPolicy=EGameplayAbilityInstancingPolicy::InstancedPerActor;
}
void UMythMeleeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* Info,
    const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* EventData)
{
    if (!CommitAbility(Handle,Info,ActivationInfo))
    {EndAbility(Handle,Info,ActivationInfo,false,true);return;}
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(Info ? Info->AvatarActor.Get():nullptr);
    if (!Owner || !Owner->GetActionComponent() || !Owner->GetActionComponent()->BeginAttack(this))
    {EndAbility(Handle,Info,ActivationInfo,false,true);}
}
void UMythMeleeAbility::CompleteFromAction()
{
    EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,false,false);
}
void UMythMeleeAbility::CancelFromAction()
{
    EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,false,true);
}
void UMythMeleeAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* Info,
    const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicate,bool bCancelled)
{
    if (AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(Info ? Info->AvatarActor.Get():nullptr))
        if (UMythActionComponent* Action=Owner->GetActionComponent()) Action->AbortAttack(this);
    Super::EndAbility(Handle,Info,ActivationInfo,bReplicate,bCancelled);
}
