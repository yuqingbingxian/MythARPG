#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MythEnemyAIController.generated.h"
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UStateTreeAIComponent;
class UEnvQuery;
struct FEnvQueryResult;
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
    /** Exposed so a StateTree Blueprint task can reuse combat logic instead of duplicating rules. */
    UFUNCTION(BlueprintCallable,Category="Myth|AI") void EvaluateCombatStep();
    /** Asynchronous EQS tactical positioning query. Call from StateTree, not every frame. */
    UFUNCTION(BlueprintCallable,Category="Myth|AI") bool RequestTacticalPosition();
    UFUNCTION(BlueprintCallable,Category="Myth|AI") bool MoveToTacticalPosition(float AcceptanceRadius=75.f);
    UFUNCTION(BlueprintCallable,Category="Myth|AI") void CancelTacticalQuery();
    UFUNCTION(BlueprintPure,Category="Myth|AI") bool HasTacticalPosition()const { return bHasTacticalPosition; }
    UFUNCTION(BlueprintPure,Category="Myth|AI") FVector GetTacticalPosition()const { return TacticalPosition; }
    UFUNCTION(BlueprintPure,Category="Myth|AI") bool IsTargetVisible()const {return bTargetVisible;}
    UFUNCTION(BlueprintPure,Category="Myth|AI") bool IsStateTreeDriven()const{return bUseStateTree;}
protected:
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)TObjectPtr<UAIPerceptionComponent> Perception;
    /** If enabled, assign a StateTree asset to the component in the AI Controller Blueprint. */
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Myth|AI|StateTree")TObjectPtr<UStateTreeAIComponent> StateTreeBrain;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Myth|AI|StateTree")bool bUseStateTree=false;
    /** Optional EQS query returning a movement destination; not required by the legacy policy. */
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Myth|AI|EQS")TObjectPtr<UEnvQuery> TacticalPositionQuery;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)TObjectPtr<UAISenseConfig_Sight> SightConfig;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Myth|AI")TObjectPtr<UMythAICombatProfile> CombatProfile;
    UPROPERTY(EditDefaultsOnly,Category="Myth|AI")bool bDebugAcquirePlayerWithoutSight=false;
    UFUNCTION()void OnTargetStimulus(AActor* Actor,FAIStimulus Stimulus);
private:
    void EvaluateCombat();
    void OnTacticalQueryFinished(TSharedPtr<FEnvQueryResult> Result);
    int32 PendingQueryId=INDEX_NONE;
    FVector TacticalPosition=FVector::ZeroVector;
    bool bHasTacticalPosition=false;
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
