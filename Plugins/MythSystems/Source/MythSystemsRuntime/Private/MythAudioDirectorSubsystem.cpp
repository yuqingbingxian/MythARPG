#include "MythAudioDirectorSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Engine/GameInstance.h"
void UMythAudioDirectorSubsystem::SetMusic(USoundBase* Music,float Volume)
{
    if(!Music || !GetWorld())return;
    StopMusic(.2f);
    ActiveMusic=UGameplayStatics::SpawnSound2D(GetWorld(),Music,FMath::Clamp(Volume,0.f,1.f),1.f,0.f,nullptr,true,false);
}
void UMythAudioDirectorSubsystem::StopMusic(float Seconds)
{
    if(ActiveMusic){ActiveMusic->FadeOut(FMath::Max(0.f,Seconds),0.f);ActiveMusic=nullptr;}
}
void UMythAudioDirectorSubsystem::PlayUISound(USoundBase* Sound)
{
    if(Sound && GetWorld())UGameplayStatics::PlaySound2D(GetWorld(),Sound);
}
