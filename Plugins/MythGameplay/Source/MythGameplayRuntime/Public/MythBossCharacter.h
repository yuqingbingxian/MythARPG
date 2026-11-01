#pragma once
#include "CoreMinimal.h"
#include "MythEnemyCharacter.h"
class UMythBossPhaseDefinition;
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
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Boss") TObjectPtr<UMythBossPhaseDefinition> PhaseDefinition;
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Boss")
    FName BossId = NAME_None;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Boss")
    FName ChapterId = NAME_None;
    UPROPERTY(BlueprintReadOnly, Category="Boss")
    int32 PhaseIndex = 1;

    UFUNCTION(BlueprintImplementableEvent, Category="Boss")
    void OnPhaseEntered(int32 NewPhase);
    UFUNCTION(BlueprintImplementableEvent,Category="Boss") void OnBossPhaseCue(FName PhaseId,FName CinematicCue);
    virtual void HandleHealthChanged(float NewHealth) override;
    virtual void HandleDeath() override;
};
