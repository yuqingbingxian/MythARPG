#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MythAttributeSet.generated.h"
#define MYTH_ATTRIBUTE_ACCESSORS(C,P) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(C,P) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(P) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(P) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(P)
UCLASS()
class MYTHGAMEPLAYRUNTIME_API UMythAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
public:
    UMythAttributeSet();
    UPROPERTY(BlueprintReadOnly,Category="Attributes") FGameplayAttributeData Health;
    MYTH_ATTRIBUTE_ACCESSORS(UMythAttributeSet,Health)
    UPROPERTY(BlueprintReadOnly,Category="Attributes") FGameplayAttributeData MaxHealth;
    MYTH_ATTRIBUTE_ACCESSORS(UMythAttributeSet,MaxHealth)
    UPROPERTY(BlueprintReadOnly,Category="Attributes") FGameplayAttributeData Stamina;
    MYTH_ATTRIBUTE_ACCESSORS(UMythAttributeSet,Stamina)
    UPROPERTY(BlueprintReadOnly,Category="Attributes") FGameplayAttributeData MaxStamina;
    MYTH_ATTRIBUTE_ACCESSORS(UMythAttributeSet,MaxStamina)
    UPROPERTY(BlueprintReadOnly,Category="Attributes") FGameplayAttributeData Poise;
    MYTH_ATTRIBUTE_ACCESSORS(UMythAttributeSet,Poise)
    UPROPERTY(BlueprintReadOnly,Category="Attributes") FGameplayAttributeData MaxPoise;
    MYTH_ATTRIBUTE_ACCESSORS(UMythAttributeSet,MaxPoise)
    UPROPERTY(BlueprintReadOnly,Category="Attributes") FGameplayAttributeData Defense;
    MYTH_ATTRIBUTE_ACCESSORS(UMythAttributeSet,Defense)
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute,float& NewValue) override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
