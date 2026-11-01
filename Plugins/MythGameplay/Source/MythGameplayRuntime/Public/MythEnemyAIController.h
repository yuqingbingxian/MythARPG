#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MythEnemyAIController.generated.h"
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UMythAICombatProfile;
class AMythCombatCharacter;

/** Real sight stimulus + target memory, leash/attack policy; BT/StateTree can query this controller. */
UCLASS()
class MYTHGAMEPLAYRUNTIME_API AMythEnemyAIController : public AAIController
{
    GENERATED_BODY()
public:
    AMythEnemyAIController();
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    UFUNCTION(BlueprintPure,Category="Myth|AI") AActor* GetCombatTarget()const{return Target.Get();}
    UFUNCTION(BlueprintPure,Category="Myth|AI") FVector GetLastKnownLocation()const{return LastKnownLocation;}
    UFUNCTION(BlueprintCallable,Category="Myth|AI") void ForgetCombatTarget();
protected:
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)TObjectPtr<UAIPerceptionComponent> Perception;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)TObjectPtr<UAISenseConfig_Sight> SightConfig;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Myth|AI")TObjectPtr<UMythAICombatProfile> CombatProfile;
    UPROPERTY(EditDefaultsOnly,Category="Myth|AI")bool bDebugAcquirePlayerWithoutSight=false;
    UFUNCTION()void OnTargetStimulus(AActor* Actor,FAIStimulus Stimulus);
private:
    void EvaluateCombat();
    FTimerHandle DecisionTimer;
    TWeakObjectPtr<AMythCombatCharacter> Target;
    FVector HomeLocation=FVector::ZeroVector;
    FVector LastKnownLocation=FVector::ZeroVector;
    double LastSeenAt=-10000.0;
    bool bTargetVisible=false;
    double LastAttackAt=-10000.0;
    TMap<FName,double> AttackCooldowns;
    FRandomStream DecisionRng;
};
