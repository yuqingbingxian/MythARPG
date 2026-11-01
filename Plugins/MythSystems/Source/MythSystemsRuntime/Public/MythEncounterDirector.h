#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MythEncounterDirector.generated.h"
class UMythEncounterDefinition;
class ATargetPoint;
class AMythCombatCharacter;
class AMythEnemyCharacter;
UENUM(BlueprintType)
enum class EMythEncounterState : uint8 { Idle, Running, Completed, Failed };
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMythEncounterStateEvent,EMythEncounterState,NewState,int32,WaveIndex);
/** Wave spawning and encounter lifetime are generic; author placement and content in Editor. */
UCLASS(Blueprintable)
class MYTHSYSTEMSRUNTIME_API AMythEncounterDirector : public AActor
{
    GENERATED_BODY()
public:
    AMythEncounterDirector();
    virtual void BeginPlay()override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;
    UPROPERTY(BlueprintAssignable,Category="Myth|Encounter")FMythEncounterStateEvent OnEncounterStateChanged;
    UFUNCTION(BlueprintCallable,Category="Myth|Encounter")bool BeginEncounter(AMythCombatCharacter* Player);
    UFUNCTION(BlueprintCallable,Category="Myth|Encounter")bool ResetEncounter();
    UFUNCTION(BlueprintPure,Category="Myth|Encounter") EMythEncounterState GetEncounterState()const{return State;}
    UFUNCTION(BlueprintPure,Category="Myth|Encounter")int32 GetCurrentWaveIndex()const{return WaveIndex;}
protected:
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Myth|Encounter")TObjectPtr<UMythEncounterDefinition> Definition;
    UPROPERTY(EditInstanceOnly,BlueprintReadOnly,Category="Myth|Encounter")TArray<TObjectPtr<ATargetPoint>> SpawnPoints;
    UPROPERTY(BlueprintReadOnly,Category="Myth|Encounter")EMythEncounterState State=EMythEncounterState::Idle;
    UPROPERTY(BlueprintReadOnly,Category="Myth|Encounter")int32 WaveIndex=INDEX_NONE;
private:
    UPROPERTY(Transient)TArray<TObjectPtr<AMythEnemyCharacter>> ActiveEnemies;
    TWeakObjectPtr<AMythCombatCharacter> PlayerRef;
    UFUNCTION()void OnEnemyDeath(AMythCombatCharacter* DeadActor);
    UFUNCTION()void OnPlayerDeath(AMythCombatCharacter* DeadActor);
    bool StartWave(int32 Index);
    bool FinishEncounter();
    void ClearSpawnedActors();
    void ChangeState(EMythEncounterState NewState);
    FName GetClearFlag()const;
};
