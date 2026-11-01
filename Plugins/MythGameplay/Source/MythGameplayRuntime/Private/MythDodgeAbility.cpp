#include "MythDodgeAbility.h"
#include "MythCombatCharacter.h"
#include "MythActionComponent.h"
UMythDodgeAbility::UMythDodgeAbility(){InstancingPolicy=EGameplayAbilityInstancingPolicy::InstancedPerActor;}
void UMythDodgeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* Info,
    const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* EventData)
{
    if (!CommitAbility(Handle,Info,ActivationInfo))
    {EndAbility(Handle,Info,ActivationInfo,false,true);return;}
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(Info ? Info->AvatarActor.Get():nullptr);
    if (!Owner || !Owner->GetActionComponent() || !Owner->GetActionComponent()->BeginDodge(this))
    {EndAbility(Handle,Info,ActivationInfo,false,true);}
}
void UMythDodgeAbility::CompleteFromAction()
{
    EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,false,false);
}
void UMythDodgeAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* Info,
    const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicate,bool bCancelled)
{
    if (AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(Info ? Info->AvatarActor.Get():nullptr))
        if (UMythActionComponent* Action=Owner->GetActionComponent()) Action->AbortDodge(this);
    Super::EndAbility(Handle,Info,ActivationInfo,bReplicate,bCancelled);
}
