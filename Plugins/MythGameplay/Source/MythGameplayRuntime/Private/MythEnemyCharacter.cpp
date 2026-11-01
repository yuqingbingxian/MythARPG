#include "MythEnemyCharacter.h"
#include "MythEnemyAIController.h"
#include "MythCombatantDefinition.h"
#include "MythEventSubsystem.h"
#include "Engine/GameInstance.h"
#include "AIController.h"

AMythEnemyCharacter::AMythEnemyCharacter()
{
    Faction = EMythFaction::Enemy;
    AIControllerClass = AMythEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

float AMythEnemyCharacter::GetAggroRange() const
{
    return CombatantDefinition ? CombatantDefinition->AggroRange : 1100.f;
}

float AMythEnemyCharacter::GetAttackRange() const
{
    return CombatantDefinition ? CombatantDefinition->AttackRange : 190.f;
}

void AMythEnemyCharacter::HandleDeath()
{
    Super::HandleDeath();
    if (AAIController* AI = Cast<AAIController>(GetController())) AI->StopMovement();
    if(bReportGenericEnemyDeath && CombatantDefinition && GetGameInstance())
        if(UMythEventSubsystem* Events=GetGameInstance()->GetSubsystem<UMythEventSubsystem>())
        {
            FMythGameplaySignal Signal;Signal.EventName=TEXT("EnemyDefeated");
            Signal.TargetId=CombatantDefinition->CombatantId;
            Events->PublishSignal(Signal);
        }
    // Do not Destroy() yet; Loot/Respawn/DeathAnimation own the lifetime.
}
