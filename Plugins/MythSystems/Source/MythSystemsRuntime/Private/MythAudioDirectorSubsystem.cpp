#include "MythAudioDirectorSubsystem.h"
#include "MythSettingsSubsystem.h"
#include "MythSettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Engine/GameInstance.h"
#include "UObject/CoreUObjectDelegates.h"
void UMythAudioDirectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UMythSettingsSubsystem>();Super::Initialize(Collection);
    if(UMythSettingsSubsystem* Pref=GetGameInstance()->GetSubsystem<UMythSettingsSubsystem>())
        Pref->OnSettingsChanged.AddDynamic(this,&UMythAudioDirectorSubsystem::HandleSettingsChanged);
    PostLoadHandle=FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&UMythAudioDirectorSubsystem::HandleMapLoaded);
}
void UMythAudioDirectorSubsystem::Deinitialize()
{
    FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadHandle);
    if(GetGameInstance())
        if(UMythSettingsSubsystem* Pref=GetGameInstance()->GetSubsystem<UMythSettingsSubsystem>())
            Pref->OnSettingsChanged.RemoveDynamic(this,&UMythAudioDirectorSubsystem::HandleSettingsChanged);
    StopMusic(0.f);Super::Deinitialize();
}
void UMythAudioDirectorSubsystem::ConfigureMix(USoundMix* Mix,USoundClass* Master,USoundClass* Music,
    USoundClass* SFX,USoundClass* Dialogue)
{
    MasterMix=Mix;MasterClass=Master;MusicClass=Music;SFXClass=SFX;DialogueClass=Dialogue;
    ApplyMixLevels();
}
void UMythAudioDirectorSubsystem::ApplyMixLevels()
{
    UWorld* World=GetWorld();
    UMythSettingsSubsystem* Pref=GetGameInstance()?GetGameInstance()->GetSubsystem<UMythSettingsSubsystem>():nullptr;
    if(!World || !Pref || !MasterMix)return;
    UGameplayStatics::PushSoundMixModifier(World,MasterMix);
    auto SetBus=[World,this](USoundClass* Class,float Volume)
    {if(Class)UGameplayStatics::SetSoundMixClassOverride(World,MasterMix,Class,FMath::Clamp(Volume,0.f,1.f),1.f,.15f,true);};
    SetBus(MasterClass,Pref->GetMasterVolume());
    SetBus(MusicClass,Pref->GetMusicVolume());
    SetBus(SFXClass,Pref->GetSFXVolume());
    SetBus(DialogueClass,Pref->GetDialogueVolume());
    if(ActiveMusic)ActiveMusic->SetVolumeMultiplier(RequestedMusicVolume);
}
void UMythAudioDirectorSubsystem::HandleSettingsChanged(){ApplyMixLevels();}
void UMythAudioDirectorSubsystem::HandleMapLoaded(UWorld* World)
{if(World==GetWorld()){ActiveMusic=nullptr;ApplyMixLevels();}}
void UMythAudioDirectorSubsystem::SetMusic(USoundBase* Music,float Volume)
{
    if(!Music || !GetWorld())return;
    StopMusic(.25f);RequestedMusicVolume=FMath::Clamp(Volume,0.f,1.f);
    ActiveMusic=UGameplayStatics::SpawnSound2D(GetWorld(),Music,RequestedMusicVolume,1.f,0.f,nullptr,true,false);
}
void UMythAudioDirectorSubsystem::StopMusic(float Seconds)
{if(ActiveMusic){ActiveMusic->FadeOut(FMath::Max(0.f,Seconds),0.f);ActiveMusic=nullptr;}}
void UMythAudioDirectorSubsystem::PlayUISound(USoundBase* Sound)
{if(Sound && GetWorld())UGameplayStatics::PlaySound2D(GetWorld(),Sound);}
