#pragma once
#include "CoreMinimal.h"
/** Placeholder balance curve; replace by signed-off combat formula later. */
struct MYTHGAMEPLAYRUNTIME_API FMythCombatMath
{
    static float MitigatedDamage(float RawDamage, float Defense)
    {
        return FMath::Max(0.f, RawDamage) * 100.f / (100.f + FMath::Max(0.f, Defense));
    }
};
