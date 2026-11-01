#include "MythCheckpointSubsystem.h"
#include "MythSaveSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Actor.h"
bool UMythCheckpointSubsystem::ActivateCheckpoint(FName Chapter,FName Checkpoint,AActor* Player,float Health)
{
    if(Chapter.IsNone() || Checkpoint.IsNone() || !Player || Health<0.f || !FMath::IsFinite(Health))return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if(S.ChapterId!=Chapter)return false;
    S.CheckpointId=Checkpoint;S.PlayerTransform=Player->GetActorTransform();
    S.PlayerHealth=Health;S.bHasPlayerState=true;
    if(!Save->SetSnapshot(S,true))return false;
    OnCheckpointActivated.Broadcast(Chapter,Checkpoint);
    return true;
}
FName UMythCheckpointSubsystem::GetActiveCheckpoint()const
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    return Save?Save->GetSnapshot().CheckpointId:NAME_None;
}
