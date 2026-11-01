#pragma once
#include "CoreMinimal.h"
#include "MythEnemyCharacter.h"
#include "MythBossCharacter.generated.h"

/** Explicit phase state + permanently defeated boss ID. */
UCLASS()
class MYTHGAMEPLAYRUNTIME_API AMythBossCharacter : public AMythEnemyCharacter
{
    GENERATED_BODY()
public:
    AMythBossCharacter();
    virtual void BeginPlay() override;
    UFUNCTION(BlueprintPure, Category="Boss")
    int32 GetPhaseIndex() const { return PhaseIndex; }
protected:
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Boss")
    FName BossId = TEXT("CH01_Boss_01");
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Boss")
    FName ChapterId = TEXT("CH01");
    UPROPERTY(BlueprintReadOnly, Category="Boss")
    int32 PhaseIndex = 1;

    UFUNCTION(BlueprintImplementableEvent, Category="Boss")
    void OnPhaseEntered(int32 NewPhase);
    virtual void HandleHealthChanged(float NewHealth) override;
    virtual void HandleDeath() override;
};
