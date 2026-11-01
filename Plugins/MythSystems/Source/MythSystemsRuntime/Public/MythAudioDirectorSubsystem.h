#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythAudioDirectorSubsystem.generated.h"
class UAudioComponent;
class USoundBase;
class USoundMix;
class USoundClass;

/** Reapplies persistent mix levels after map load and preference changes. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythAudioDirectorSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    UFUNCTION(BlueprintCallable) void ConfigureMix(USoundMix* InMix,USoundClass* InMaster,
        USoundClass* InMusic,USoundClass* InSFX,USoundClass* InDialogue);
    UFUNCTION(BlueprintCallable) void SetMusic(USoundBase* Music,float Volume=1.f);
    UFUNCTION(BlueprintCallable) void StopMusic(float FadeOutSeconds=.5f);
    UFUNCTION(BlueprintCallable) void PlayUISound(USoundBase* Sound);
    UFUNCTION(BlueprintCallable) void ApplyMixLevels();
private:
    UPROPERTY(Transient) TObjectPtr<UAudioComponent> ActiveMusic;
    UPROPERTY(Transient) TObjectPtr<USoundMix> MasterMix;
    UPROPERTY(Transient) TObjectPtr<USoundClass> MasterClass;
    UPROPERTY(Transient) TObjectPtr<USoundClass> MusicClass;
    UPROPERTY(Transient) TObjectPtr<USoundClass> SFXClass;
    UPROPERTY(Transient) TObjectPtr<USoundClass> DialogueClass;
    FDelegateHandle PostLoadHandle;
    float RequestedMusicVolume=1.f;
    bool bMixPushed=false;
    UFUNCTION() void HandleSettingsChanged();
    void HandleMapLoaded(UWorld* World);
};
