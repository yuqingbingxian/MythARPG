#include "MythHUDViewModel.h"
#include "MythCombatCharacter.h"
#include "MythAttributeSet.h"
#include "AbilitySystemComponent.h"
void UMythHUDViewModel::BindCombatant(AMythCombatCharacter* Combatant)
{
    UnbindCombatant();Subject=Combatant;
    UAbilitySystemComponent* ASC=Combatant?Combatant->GetAbilitySystemComponent():nullptr;
    if(ASC)
    {
        ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetHealthAttribute()).AddUObject(this,&UMythHUDViewModel::OnAttributeChanged);
        ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetMaxHealthAttribute()).AddUObject(this,&UMythHUDViewModel::OnAttributeChanged);
        ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetStaminaAttribute()).AddUObject(this,&UMythHUDViewModel::OnAttributeChanged);
        ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetMaxStaminaAttribute()).AddUObject(this,&UMythHUDViewModel::OnAttributeChanged);
        ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetPoiseAttribute()).AddUObject(this,&UMythHUDViewModel::OnAttributeChanged);
        ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetMaxPoiseAttribute()).AddUObject(this,&UMythHUDViewModel::OnAttributeChanged);
    }
    OnStatsChanged.Broadcast();
}
void UMythHUDViewModel::UnbindCombatant()
{
    if(AMythCombatCharacter* Combatant=Subject.Get())
        if(UAbilitySystemComponent* ASC=Combatant->GetAbilitySystemComponent())
        {
            ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetHealthAttribute()).RemoveAll(this);
            ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetMaxHealthAttribute()).RemoveAll(this);
            ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetStaminaAttribute()).RemoveAll(this);
            ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetMaxStaminaAttribute()).RemoveAll(this);
            ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetPoiseAttribute()).RemoveAll(this);
            ASC->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetMaxPoiseAttribute()).RemoveAll(this);
        }
    Subject.Reset();
}
void UMythHUDViewModel::BeginDestroy(){UnbindCombatant();Super::BeginDestroy();}
void UMythHUDViewModel::OnAttributeChanged(const FOnAttributeChangeData& Data){OnStatsChanged.Broadcast();}
float UMythHUDViewModel::HealthFraction()const
{return Subject.IsValid() && Subject->GetMaxHealth()>0.f?FMath::Clamp(Subject->GetCurrentHealth()/Subject->GetMaxHealth(),0.f,1.f):0.f;}
float UMythHUDViewModel::StaminaFraction()const
{
    const AMythCombatCharacter* C=Subject.Get();
    const UAbilitySystemComponent* A=C?C->GetAbilitySystemComponent():nullptr;
    const float Max=A?A->GetNumericAttribute(UMythAttributeSet::GetMaxStaminaAttribute()):0.f;
    return Max>0.f?FMath::Clamp(C->GetCurrentStamina()/Max,0.f,1.f):0.f;
}
float UMythHUDViewModel::PoiseFraction()const
{
    const AMythCombatCharacter* C=Subject.Get();
    const UAbilitySystemComponent* A=C?C->GetAbilitySystemComponent():nullptr;
    const float Max=A?A->GetNumericAttribute(UMythAttributeSet::GetMaxPoiseAttribute()):0.f;
    return Max>0.f?FMath::Clamp(C->GetCurrentPoise()/Max,0.f,1.f):0.f;
}
bool UMythHUDViewModel::IsCharacterDead()const{return !Subject.IsValid() || Subject->IsDead();}
