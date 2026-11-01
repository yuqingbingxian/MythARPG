#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MythExecutionComponent.generated.h"
class AMythCombatCharacter;
class UAnimMontage;
class UMotionWarpingComponent;

/** Two-actor synchronized execution. Choreography is separate from damage/loot. */
UCLASS(ClassGroup=(Myth),meta=(BlueprintSpawnableComponent))
class MYTHGAMEPLAYRUNTIME_API UMythExecutionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UMythExecutionComponent();
    UFUNCTION(BlueprintPure,Category="Myth|Execution") bool IsBusy() const {return bBusy;}
    UFUNCTION(BlueprintCallable,Category="Myth|Execution") bool BeginExecution(
        AMythCombatCharacter* Victim,UAnimMontage* AttackerMontage,UAnimMontage* VictimMontage,
        FName WarpTargetName,float MaxDistance=250.f);
    /** Call once from an authored execution impact notify, NOT from the montage start. */
    UFUNCTION(BlueprintCallable,Category="Myth|Execution") bool CommitImpact(float DamageScale=4.f,float PoiseDamage=100.f);
    UFUNCTION(BlueprintCallable,Category="Myth|Execution") void CancelExecution();
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
    UPROPERTY(Transient) TObjectPtr<UAnimMontage> AttackerMontageActive;
    UPROPERTY(Transient) TObjectPtr<UAnimMontage> VictimMontageActive;
    TWeakObjectPtr<AMythCombatCharacter> VictimActor;
    TWeakObjectPtr<UMythExecutionComponent> Partner;
    FName ActiveWarpTarget=NAME_None;
    bool bBusy=false;
    bool bAppliedImpact=false;
    bool bCleaning=false;
    void OnAttackerMontageEnded(UAnimMontage* Montage,bool bInterrupted);
    void OnVictimMontageEnded(UAnimMontage* Montage,bool bInterrupted);
    void ReleasePair(bool bStopMontages);
};
