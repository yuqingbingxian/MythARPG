#pragma once
#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "MythDamageEffect.generated.h"
/** Two negative set-by-caller resource deltas: Health and Poise. */
UCLASS()
class MYTHGAMEPLAYRUNTIME_API UMythDamageEffect : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UMythDamageEffect();
};
