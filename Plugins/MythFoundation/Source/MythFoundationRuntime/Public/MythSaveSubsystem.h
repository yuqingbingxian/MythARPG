#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythSaveSubsystem.generated.h"

class UMythSaveGame;

/** Synchronous prototype slot. Replace with snapshot + async writer before shipping. */
UCLASS()
class MYTHFOUNDATIONRUNTIME_API UMythSaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Save")
    bool SavePlayerState(const FTransform& Transform, float Health);

    UFUNCTION(BlueprintCallable, Category="Save")
    bool RecordBossDefeat(FName BossId);

    UFUNCTION(BlueprintPure, Category="Save")
    bool IsBossDefeated(FName BossId) const;

    UFUNCTION(BlueprintPure, Category="Save")
    bool GetSavedPlayerState(FTransform& OutTransform, float& OutHealth) const;

    UFUNCTION(BlueprintCallable, Category="Save")
    bool ReloadFromDisk();

private:
    bool CommitToDisk();
    UPROPERTY(Transient)
    TObjectPtr<UMythSaveGame> CurrentSave;
    const FString SlotName = TEXT("Campaign_00");
};
