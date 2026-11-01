#include "MythEnemyAIController.h"
#include "MythCombatDirectorSubsystem.h"
#include "MythEnemyCharacter.h"
#include "MythPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

AMythEnemyAIController::AMythEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMythEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    GetWorldTimerManager().SetTimer(DecisionTimer, this,
        &AMythEnemyAIController::EvaluateCombat, 0.25f, true);
}

void AMythEnemyAIController::OnUnPossess()
{
    GetWorldTimerManager().ClearTimer(DecisionTimer);
    if (UWorld* W=GetWorld()) if (UMythCombatDirectorSubsystem* Director=W->GetSubsystem<UMythCombatDirectorSubsystem>()) Director->ReleaseAttack(GetPawn());
    Super::OnUnPossess();
}

void AMythEnemyAIController::EvaluateCombat()
{
    AMythEnemyCharacter* Enemy = Cast<AMythEnemyCharacter>(GetPawn());
    AMythPlayerCharacter* Player = Cast<AMythPlayerCharacter>(
        UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Enemy || !Player || Enemy->IsDead() || Player->IsDead())
    {
        StopMovement();
        return;
    }
    const float Distance = FVector::Dist2D(Enemy->GetActorLocation(), Player->GetActorLocation());
    if (Distance > Enemy->GetAggroRange())
    {
        StopMovement();
        return;
    }
    if (Distance > Enemy->GetAttackRange())
    {
        MoveToActor(Player, Enemy->GetAttackRange() * 0.8f);
        return;
    }
    StopMovement();
    const FVector Direction = Player->GetActorLocation() - Enemy->GetActorLocation();
    if (!Direction.IsNearlyZero()) Enemy->SetActorRotation(Direction.Rotation());
    if (GetWorld()->GetTimeSeconds() - LastAttackTime >= 1.25f)
    {
        UMythCombatDirectorSubsystem* Director=GetWorld()->GetSubsystem<UMythCombatDirectorSubsystem>();
        if(!Director || Director->TryReserveAttack(Enemy,Player,1.5f))
        {
            if (Enemy->RequestMeleeAbility()) LastAttackTime=GetWorld()->GetTimeSeconds();
            else if(Director)Director->ReleaseAttack(Enemy);
        }
    }
}
