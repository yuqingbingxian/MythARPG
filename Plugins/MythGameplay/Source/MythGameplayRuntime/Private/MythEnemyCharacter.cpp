#include "MythEnemyCharacter.h"
#include "MythEnemyAIController.h"
#include "MythCombatantDefinition.h"
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
    // Do not Destroy() yet; future Loot/Respawn/DeathAnimation need explicit state transitions.
}
