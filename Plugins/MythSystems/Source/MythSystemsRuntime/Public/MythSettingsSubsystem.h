#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythSettingsSubsystem.generated.h"
class UMythSettingsSaveGame;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMythSettingsChanged);

/** Persistent non-campaign preferences. Soundmix/camera effects bind to OnSettingsChanged. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythSettingsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    UPROPERTY(BlueprintAssignable) FMythSettingsChanged OnSettingsChanged;
    UFUNCTION(BlueprintPure) bool GetSubtitlesEnabled()const;
    UFUNCTION(BlueprintPure) bool GetInvertCameraY()const;
    UFUNCTION(BlueprintPure) bool GetReducedCameraShake()const;
    UFUNCTION(BlueprintPure) float GetCameraSensitivity()const;
    UFUNCTION(BlueprintPure) float GetMasterVolume()const;
    UFUNCTION(BlueprintPure) float GetMusicVolume()const;
    UFUNCTION(BlueprintPure) float GetSFXVolume()const;
    UFUNCTION(BlueprintPure) float GetDialogueVolume()const;
    UFUNCTION(BlueprintCallable) bool SetAccessibility(bool bSubtitles,bool bInvertY,bool bReduceShake,float Sensitivity);
    UFUNCTION(BlueprintCallable) bool SetAudioLevels(float Master,float Music,float Sfx,float Dialogue);
    UFUNCTION(BlueprintCallable) bool SavePreferences();
    UFUNCTION(BlueprintCallable) void ApplyVideoQuality(int32 QualityLevel,bool bConfirmVideoMode);
private:
    UPROPERTY(Transient)TObjectPtr<UMythSettingsSaveGame> Preferences;
    static constexpr const TCHAR* Slot=TEXT("MythLocalSettings");
};
