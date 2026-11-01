#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "GameplayTagContainer.h"
#include "MythBossPhaseDefinition.generated.h"

/** Health threshold is normalized 0..1. List phases in encounter order. */
USTRUCT(BlueprintType)
struct FMythBossPhase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName PhaseId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="0",ClampMax="1")) float EnterAtOrBelowHealthRatio=1.f;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FGameplayTagContainer AttackProfileTags;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName CinematicCue=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) bool bAllowSkipPhase=false;
};
UCLASS(BlueprintType)
class MYTHGAMEPLAYRUNTIME_API UMythBossPhaseDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName EncounterId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythBossPhase> Phases;
    bool ValidatePhases(FString& Error)const;
    int32 CalculatePhaseIndex(float HealthRatio,int32 CurrentIndex)const;
};
