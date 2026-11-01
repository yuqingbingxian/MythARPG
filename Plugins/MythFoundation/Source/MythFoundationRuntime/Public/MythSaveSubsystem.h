#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythCampaignTypes.h"
#include "MythSaveSubsystem.generated.h"
class UMythSaveGame;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMythSaveFinished,bool,bSuccess);

/** One owner for campaign state. All transactions operate on a game-thread snapshot. */
UCLASS()
class MYTHFOUNDATIONRUNTIME_API UMythSaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    UPROPERTY(BlueprintAssignable,Category="Save") FMythSaveFinished OnSaveFinished;
    UFUNCTION(BlueprintCallable,Category="Save") bool ReloadFromDisk();
    UFUNCTION(BlueprintPure,Category="Save") FMythCampaignSnapshot GetSnapshot() const;
    /** Stage a complete validated snapshot. Set bFlush immediately only at safe boundaries. */
    UFUNCTION(BlueprintCallable,Category="Save") bool SetSnapshot(const FMythCampaignSnapshot& NewSnapshot, bool bFlushNow = false);
    UFUNCTION(BlueprintCallable,Category="Save") bool FlushToDisk();
    UFUNCTION(BlueprintCallable,Category="Save") bool RequestAsyncSave();
    UFUNCTION(BlueprintPure,Category="Save") bool IsDirty() const { return bDirty; }
    UFUNCTION(BlueprintPure,Category="Save") bool IsSaveInProgress() const { return bSaving; }
    // Kept for compatibility with the v0.2 character/Boss APIs.
    UFUNCTION(BlueprintCallable,Category="Save") bool SavePlayerState(const FTransform& Transform,float Health);
    UFUNCTION(BlueprintCallable,Category="Save") bool RecordBossDefeat(FName BossId);
    UFUNCTION(BlueprintPure,Category="Save") bool IsBossDefeated(FName BossId) const;
    UFUNCTION(BlueprintPure,Category="Save") bool GetSavedPlayerState(FTransform& OutTransform,float& OutHealth) const;
private:
    UPROPERTY(Transient) TObjectPtr<UMythSaveGame> CurrentSave;
    /** In-flight save bytes are owned by the async worker, never by a mutable UObject. */
    bool bDirty = false;
    bool bSaving = false;
    bool bQueuedSave = false;
    uint64 MutationRevision = 0;
    uint64 InFlightRevision = 0;
    const FString SlotName = TEXT("Campaign_00");
    bool ValidateSnapshot(const FMythCampaignSnapshot& Value) const;
    void MigrateLoadedSave();
    bool SerializeSnapshot(TArray<uint8>& OutBytes) const;
    void OnAsyncSaveCompleted(bool bSuccess);
};
