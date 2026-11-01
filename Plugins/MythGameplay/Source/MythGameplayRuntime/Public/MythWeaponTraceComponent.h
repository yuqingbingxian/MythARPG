#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MythWeaponTraceComponent.generated.h"
class AMythCombatCharacter;

/** Active only during the attack's hit window. Sweeps socket motion, deduplicates each window. */
UCLASS(ClassGroup=(Myth), meta=(BlueprintSpawnableComponent))
class MYTHGAMEPLAYRUNTIME_API UMythWeaponTraceComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UMythWeaponTraceComponent();
    void BeginTrace(float InDamageScale, float InPoiseDamage);
    void EndTrace();
    virtual void TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTick) override;
protected:
    UPROPERTY(EditAnywhere, Category="WeaponTrace") TArray<FName> TraceSockets;
    UPROPERTY(EditAnywhere, Category="WeaponTrace", meta=(ClampMin="1")) float TraceRadius = 34.f;
    UPROPERTY(EditAnywhere, Category="WeaponTrace", meta=(ClampMin="1", ClampMax="12")) int32 MaxSubsteps = 5;
private:
    bool bActive = false;
    float DamageScale = 1.f;
    float PoiseDamage = 20.f;
    TArray<FVector> Previous;
    TSet<TWeakObjectPtr<AActor>> HitActors;
    TArray<FVector> SamplePositions() const;
    void SweepBetween(const FVector& From, const FVector& To);
};
