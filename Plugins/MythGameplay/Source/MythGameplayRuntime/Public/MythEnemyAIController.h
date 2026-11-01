#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "TimerManager.h"
#include "MythEnemyAIController.generated.h"

/** Fixed-rate prototype AI; replaced with Perception + StateTree after combat slice. */
UCLASS()
class MYTHGAMEPLAYRUNTIME_API AMythEnemyAIController : public AAIController
{
    GENERATED_BODY()
public:
    AMythEnemyAIController();
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
private:
    void EvaluateCombat();
    FTimerHandle DecisionTimer;
    float LastAttackTime = -1000.f;
};
