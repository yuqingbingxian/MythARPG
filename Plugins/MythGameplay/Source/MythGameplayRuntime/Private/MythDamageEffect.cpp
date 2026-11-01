#include "MythDamageEffect.h"
#include "MythAttributeSet.h"
UMythDamageEffect::UMythDamageEffect()
{
    DurationPolicy=EGameplayEffectDurationType::Instant;
    auto AddDelta=[this](const FGameplayAttribute& A,FName CallerName)
    {
        FGameplayModifierInfo Modifier;
        Modifier.Attribute=A;Modifier.ModifierOp=EGameplayModOp::Additive;
        FSetByCallerFloat Caller;Caller.DataName=CallerName;
        Modifier.ModifierMagnitude=FGameplayEffectModifierMagnitude(Caller);
        Modifiers.Add(Modifier);
    };
    AddDelta(UMythAttributeSet::GetHealthAttribute(),TEXT("Damage"));
    AddDelta(UMythAttributeSet::GetPoiseAttribute(),TEXT("PoiseDamage"));
}
