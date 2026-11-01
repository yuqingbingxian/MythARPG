#include "MythSettingsSubsystem.h"
#include "MythSettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
void UMythSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    Preferences=Cast<UMythSettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot,0));
    if(!Preferences || Preferences->SchemaVersion!=1)
        Preferences=Cast<UMythSettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(UMythSettingsSaveGame::StaticClass()));
}
bool UMythSettingsSubsystem::GetSubtitlesEnabled()const{return Preferences?Preferences->bSubtitlesEnabled:true;}
bool UMythSettingsSubsystem::GetInvertCameraY()const{return Preferences?Preferences->bCameraInvertY:false;}
bool UMythSettingsSubsystem::GetReducedCameraShake()const{return Preferences?Preferences->bReducedCameraShake:false;}
float UMythSettingsSubsystem::GetCameraSensitivity()const{return Preferences?Preferences->CameraSensitivity:1.f;}
float UMythSettingsSubsystem::GetMasterVolume()const{return Preferences?Preferences->MasterVolume:1.f;}
float UMythSettingsSubsystem::GetMusicVolume()const{return Preferences?Preferences->MusicVolume:.8f;}
float UMythSettingsSubsystem::GetSFXVolume()const{return Preferences?Preferences->SfxVolume:1.f;}
float UMythSettingsSubsystem::GetDialogueVolume()const{return Preferences?Preferences->DialogueVolume:1.f;}
bool UMythSettingsSubsystem::SavePreferences()
{return Preferences && UGameplayStatics::SaveGameToSlot(Preferences,Slot,0);}
bool UMythSettingsSubsystem::SetAccessibility(bool Subtitles,bool InvertY,bool ReduceShake,float Sensitivity)
{
    if(!Preferences || !FMath::IsFinite(Sensitivity))return false;
    Preferences->bSubtitlesEnabled=Subtitles;Preferences->bCameraInvertY=InvertY;
    Preferences->bReducedCameraShake=ReduceShake;
    Preferences->CameraSensitivity=FMath::Clamp(Sensitivity,0.1f,5.f);
    OnSettingsChanged.Broadcast();return SavePreferences();
}
bool UMythSettingsSubsystem::SetAudioLevels(float Master,float Music,float Sfx,float Dialogue)
{
    if(!Preferences || !FMath::IsFinite(Master) || !FMath::IsFinite(Music) ||
       !FMath::IsFinite(Sfx) || !FMath::IsFinite(Dialogue))return false;
    Preferences->MasterVolume=FMath::Clamp(Master,0.f,1.f);
    Preferences->MusicVolume=FMath::Clamp(Music,0.f,1.f);
    Preferences->SfxVolume=FMath::Clamp(Sfx,0.f,1.f);
    Preferences->DialogueVolume=FMath::Clamp(Dialogue,0.f,1.f);
    OnSettingsChanged.Broadcast();return SavePreferences();
}
void UMythSettingsSubsystem::ApplyVideoQuality(int32 Quality,bool bConfirm)
{
    UGameUserSettings* Settings=GEngine?GEngine->GetGameUserSettings():nullptr;
    if(!Settings)return;
    Settings->SetOverallScalabilityLevel(FMath::Clamp(Quality,0,4));
    Settings->ApplySettings(false);
    if(bConfirm)Settings->ConfirmVideoMode();
    Settings->SaveSettings();
}
