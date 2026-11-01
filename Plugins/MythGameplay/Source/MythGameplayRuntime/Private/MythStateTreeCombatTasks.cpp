#include "MythStateTreeCombatTasks.h"
#include "MythEnemyAIController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FMythCombatDecisionTask::EnterState(FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition)const
{
    FInstanceDataType& Data=Context.GetInstanceData(*this);
    Data.Elapsed=0.f;
    if(!IsValid(Data.Controller))return EStateTreeRunStatus::Failed;
    Data.Controller->EvaluateCombatStep();
    return EStateTreeRunStatus::Running;
}
EStateTreeRunStatus FMythCombatDecisionTask::Tick(FStateTreeExecutionContext& Context,float DeltaTime)const
{
    FInstanceDataType& Data=Context.GetInstanceData(*this);
    if(!IsValid(Data.Controller) || !IsValid(Data.Controller->GetPawn()))return EStateTreeRunStatus::Failed;
    Data.Elapsed+=FMath::Max(0.f,DeltaTime);
    if(Data.Elapsed>=FMath::Max(.05f,Data.DecisionPeriod))
    {
        Data.Elapsed=0.f;Data.Controller->EvaluateCombatStep();
    }
    return EStateTreeRunStatus::Running;
}
EStateTreeRunStatus FMythTacticalMoveTask::EnterState(FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition)const
{
    FInstanceDataType& Data=Context.GetInstanceData(*this);
    Data.Elapsed=0.f;Data.bMoving=false;
    return IsValid(Data.Controller) && Data.Controller->RequestTacticalPosition()
        ? EStateTreeRunStatus::Running:EStateTreeRunStatus::Failed;
}
EStateTreeRunStatus FMythTacticalMoveTask::Tick(FStateTreeExecutionContext& Context,float DeltaTime)const
{
    FInstanceDataType& Data=Context.GetInstanceData(*this);
    if(!IsValid(Data.Controller) || !IsValid(Data.Controller->GetPawn()))return EStateTreeRunStatus::Failed;
    Data.Elapsed+=FMath::Max(0.f,DeltaTime);
    if(Data.Controller->HasTacticalPosition() && !Data.bMoving)
    {
        Data.bMoving=Data.Controller->MoveToTacticalPosition(Data.AcceptanceRadius);
        return Data.bMoving?EStateTreeRunStatus::Succeeded:EStateTreeRunStatus::Failed;
    }
    return Data.Elapsed>=Data.QueryTimeout?EStateTreeRunStatus::Failed:EStateTreeRunStatus::Running;
}

void FMythTacticalMoveTask::ExitState(FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition)const
{
    FInstanceDataType& Data=Context.GetInstanceData(*this);
    if(IsValid(Data.Controller) && !Data.bMoving)Data.Controller->CancelTacticalQuery();
}
