#include "MythAttributeSet.h"
#include "GameplayEffectExtension.h"
UMythAttributeSet::UMythAttributeSet()
{
    InitMaxHealth(100.f);InitHealth(100.f);InitMaxStamina(100.f);InitStamina(100.f);
    InitMaxPoise(100.f);InitPoise(100.f);InitDefense(0.f);
}
void UMythAttributeSet::PreAttributeChange(const FGameplayAttribute& A,float& V)
{
    Super::PreAttributeChange(A,V);
    if (A==GetHealthAttribute()) V=FMath::Clamp(V,0.f,GetMaxHealth());
    if (A==GetStaminaAttribute()) V=FMath::Clamp(V,0.f,GetMaxStamina());
    if (A==GetPoiseAttribute()) V=FMath::Clamp(V,0.f,GetMaxPoise());
    if (A==GetDefenseAttribute()) V=FMath::Max(0.f,V);
}
void UMythAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    const FGameplayAttribute& A=Data.EvaluatedData.Attribute;
    if (A==GetHealthAttribute())SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));
    else if(A==GetStaminaAttribute())SetStamina(FMath::Clamp(GetStamina(),0.f,GetMaxStamina()));
    else if(A==GetPoiseAttribute())SetPoise(FMath::Clamp(GetPoise(),0.f,GetMaxPoise()));
}
