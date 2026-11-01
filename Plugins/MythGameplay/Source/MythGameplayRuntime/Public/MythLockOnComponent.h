#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MythLockOnComponent.generated.h"
class AMythCombatCharacter;

/** Player-only target acquisition, target lifetime validation and camera yaw/pitch tracking. */
UCLASS(ClassGroup=(Myth),meta=(BlueprintSpawnableComponent))
class MYTHGAMEPLAYRUNTIME_API UMythLockOnComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UMythLockOnComponent();
    UFUNCTION(BlueprintCallable,Category="Myth|LockOn") bool ToggleLockOn();
    UFUNCTION(BlueprintCallable,Category="Myth|LockOn") void ClearLockOn();
    UFUNCTION(BlueprintCallable,Category="Myth|LockOn") bool CycleTarget(bool bRight);
    UFUNCTION(BlueprintPure,Category="Myth|LockOn") AMythCombatCharacter* GetTarget() const {return Target.Get();}
    virtual void TickComponent(float DeltaTime,ELevelTick TickType,FActorComponentTickFunction* Func) override;
protected:
    UPROPERTY(EditDefaultsOnly,Category="Myth|LockOn") float SearchRadius=1850.f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|LockOn") float MaximumLockedDistance=2300.f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|LockOn") float ViewDotMinimum=.1f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|LockOn") float CameraRotationSpeed=9.f;
private:
    TWeakObjectPtr<AMythCombatCharacter> Target;
    bool bOldOrientToMovement=true;
    bool bOldControllerYaw=false;
    TArray<AMythCombatCharacter*> CollectTargets() const;
    bool IsCandidate(AMythCombatCharacter* Candidate) const;
    void LockTarget(AMythCombatCharacter* Candidate);
};
