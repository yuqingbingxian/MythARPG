#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythEncounterDefinition.generated.h"
class AMythEnemyCharacter;
class UMythRewardDefinition;
USTRUCT(BlueprintType)
struct FMythEncounterSpawn
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TSoftClassPtr<AMythEnemyCharacter> EnemyClass;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="1",ClampMax="32")) int32 Count=1;
};
USTRUCT(BlueprintType)
struct FMythEncounterWave
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName WaveId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) TArray<FMythEncounterSpawn> Spawns;
};

/** Data-only combat encounter definition; no region or Chapter01 dependency. */
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythEncounterDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName EncounterId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName ChapterId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) bool bPermanentClear=true;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythEncounterWave> Waves;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TSoftObjectPtr<UMythRewardDefinition> ClearReward;
    UFUNCTION(BlueprintPure)bool Validate(FString& Error)const;
    virtual FPrimaryAssetId GetPrimaryAssetId()const override
    {return FPrimaryAssetId(TEXT("Encounter"), EncounterId.IsNone()?GetFName():EncounterId);}
};
