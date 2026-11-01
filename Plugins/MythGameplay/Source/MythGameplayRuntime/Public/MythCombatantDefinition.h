#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythCombatantDefinition.generated.h"
UCLASS(BlueprintType)
class MYTHGAMEPLAYRUNTIME_API UMythCombatantDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Identity") FName CombatantId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Stats",meta=(ClampMin="1"))float MaxHealth=100.f;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Stats",meta=(ClampMin="1"))float MaxStamina=100.f;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Stats",meta=(ClampMin="1"))float MaxPoise=100.f;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Stats",meta=(ClampMin="0"))float StaminaRegenPerSecond=22.f;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Stats",meta=(ClampMin="0"))float Defense=0.f;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat",meta=(ClampMin="0"))float MeleeDamage=20.f;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AI",meta=(ClampMin="100"))float AggroRange=1100.f;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AI",meta=(ClampMin="75"))float AttackRange=190.f;
    virtual FPrimaryAssetId GetPrimaryAssetId()const override
    {return FPrimaryAssetId(TEXT("Combatant"),CombatantId.IsNone()?GetFName():CombatantId);}
};
