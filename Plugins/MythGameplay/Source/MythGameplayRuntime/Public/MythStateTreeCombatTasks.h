#pragma once
#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MythStateTreeCombatTasks.generated.h"
class AMythEnemyAIController;

/** StateTree binds Controller to the owning AI controller in the task Details panel. */
USTRUCT(BlueprintType)
struct MYTHGAMEPLAYRUNTIME_API FMythCombatDecisionTaskData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,Category="Context") TObjectPtr<AMythEnemyAIController> Controller=nullptr;
    UPROPERTY(EditAnywhere,Category="Task",meta=(ClampMin="0.05"))float DecisionPeriod=0.25f;
    float Elapsed=0.f;
};

/** Native task: invokes the same attack selection used by non-StateTree enemies. */
USTRUCT(meta=(DisplayName="Myth: Combat decision"))
struct MYTHGAMEPLAYRUNTIME_API FMythCombatDecisionTask : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()
    using FInstanceDataType=FMythCombatDecisionTaskData;
    virtual const UStruct* GetInstanceDataType() const override{return FInstanceDataType::StaticStruct();}
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,const FStateTreeTransitionResult& Transition) const override;
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,const float DeltaTime) const override;
};

USTRUCT(BlueprintType)
struct MYTHGAMEPLAYRUNTIME_API FMythTacticalMoveTaskData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,Category="Context") TObjectPtr<AMythEnemyAIController> Controller=nullptr;
    UPROPERTY(EditAnywhere,Category="Task",meta=(ClampMin="0.1"))float QueryTimeout=2.f;
    UPROPERTY(EditAnywhere,Category="Task",meta=(ClampMin="5"))float AcceptanceRadius=75.f;
    float Elapsed=0.f;
    bool bMoving=false;
};

/** Wait for an async EQS query, then request navigation to its result. */
USTRUCT(meta=(DisplayName="Myth: Request tactical position"))
struct MYTHGAMEPLAYRUNTIME_API FMythTacticalMoveTask : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()
    using FInstanceDataType=FMythTacticalMoveTaskData;
    virtual const UStruct* GetInstanceDataType() const override{return FInstanceDataType::StaticStruct();}
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,const FStateTreeTransitionResult& Transition) const override;
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,const float DeltaTime) const override;
};
