#pragma once
#include "CoreMinimal.h"
#include "MythCombatCharacter.h"
#include "MythEnemyCharacter.generated.h"

UCLASS()
class MYTHGAMEPLAYRUNTIME_API AMythEnemyCharacter : public AMythCombatCharacter
{
    GENERATED_BODY()
public:
    AMythEnemyCharacter();
    float GetAggroRange() const;
    float GetAttackRange() const;
protected:
    UPROPERTY(EditDefaultsOnly,Category="AI") bool bReportGenericEnemyDeath=true;
    virtual void HandleDeath() override;
};
