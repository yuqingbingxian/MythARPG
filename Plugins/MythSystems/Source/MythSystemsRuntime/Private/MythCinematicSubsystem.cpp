#include "MythCinematicSubsystem.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "UObject/CoreUObjectDelegates.h"

void UMythCinematicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PreMapHandle=FCoreUObjectDelegates::PreLoadMap.AddUObject(this,&UMythCinematicSubsystem::BeforeMapLoad);
}
void UMythCinematicSubsystem::Deinitialize()
{
    FCoreUObjectDelegates::PreLoadMap.Remove(PreMapHandle);
    BeforeMapLoad(TEXT("shutdown"));
    Super::Deinitialize();
}
bool UMythCinematicSubsystem::PlayCinematic(FName CinematicId,ULevelSequence* Sequence)
{
    if(!Sequence || CinematicId.IsNone() || ActivePlayer || !GetWorld())return false;
    FMovieSceneSequencePlaybackSettings Settings;
    ALevelSequenceActor* Spawned=nullptr;
    ULevelSequencePlayer* Player=ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(),Sequence,Settings,Spawned);
    if(!Player || !Spawned)return false;
    ActivePlayer=Player;ActiveActor=Spawned;ActiveId=CinematicId;bSkipping=false;
    Player->OnFinished.AddDynamic(this,&UMythCinematicSubsystem::OnSequenceFinished);
    if(APlayerController* PC=GetGameInstance()->GetFirstLocalPlayerController())
    {PC->SetIgnoreMoveInput(true);PC->SetIgnoreLookInput(true);}
    Player->Play();return true;
}
bool UMythCinematicSubsystem::SkipCinematic()
{
    if(!ActivePlayer)return false;
    bSkipping=true;
    ActivePlayer->GoToEndAndStop();
    Finish(true);return true;
}
void UMythCinematicSubsystem::OnSequenceFinished(){Finish(bSkipping);}
void UMythCinematicSubsystem::Finish(bool bSkipped)
{
    if(!ActivePlayer)return;
    ULevelSequencePlayer* Player=ActivePlayer;
    ALevelSequenceActor* Actor=ActiveActor;
    const FName FinishedId=ActiveId;
    ActivePlayer=nullptr;ActiveActor=nullptr;ActiveId=NAME_None;bSkipping=false;
    Player->OnFinished.RemoveDynamic(this,&UMythCinematicSubsystem::OnSequenceFinished);
    if(APlayerController* PC=GetGameInstance()?GetGameInstance()->GetFirstLocalPlayerController():nullptr)
    {PC->SetIgnoreMoveInput(false);PC->SetIgnoreLookInput(false);}
    if(Actor && IsValid(Actor))Actor->Destroy();
    OnCinematicFinished.Broadcast(FinishedId,bSkipped);
}
void UMythCinematicSubsystem::BeforeMapLoad(const FString& MapName)
{
    if(ActivePlayer)ActivePlayer->Stop();
    // Do not credit interrupted cutscenes as completed: the caller must reconcile story state.
    if(ActivePlayer)
    {
        ULevelSequencePlayer* Player=ActivePlayer;
        ALevelSequenceActor* Actor=ActiveActor;
        ActivePlayer=nullptr;ActiveActor=nullptr;ActiveId=NAME_None;
        Player->OnFinished.RemoveDynamic(this,&UMythCinematicSubsystem::OnSequenceFinished);
        if(APlayerController* PC=GetGameInstance()?GetGameInstance()->GetFirstLocalPlayerController():nullptr)
        {PC->SetIgnoreMoveInput(false);PC->SetIgnoreLookInput(false);}
        if(Actor && IsValid(Actor))Actor->Destroy();
    }
}
