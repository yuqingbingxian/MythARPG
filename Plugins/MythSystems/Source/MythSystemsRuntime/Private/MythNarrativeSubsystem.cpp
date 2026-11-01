#include "MythNarrativeSubsystem.h"
#include "MythDialogueDefinition.h"
#include "MythSaveSubsystem.h"
#include "Engine/GameInstance.h"
FName UMythNarrativeSubsystem::CurrentDialogueId()const
{return ActiveDialogue?ActiveDialogue->DialogueId:NAME_None;}
bool UMythNarrativeSubsystem::StartDialogue(UMythDialogueDefinition* Definition)
{
    if(!Definition || ActiveDialogue || !Definition->HasNode(Definition->StartNodeId))return false;
    ActiveDialogue=Definition;
    ActiveNodeId=Definition->StartNodeId;
    OnNodeEntered.Broadcast(CurrentDialogueId(),ActiveNodeId);
    return true;
}
bool UMythNarrativeSubsystem::CanChoose(int32 Index)const
{
    const FMythDialogueNode* N=ActiveDialogue?ActiveDialogue->FindNode(ActiveNodeId):nullptr;
    if(!N || !N->Choices.IsValidIndex(Index))return false;
    const FMythDialogueChoice& Choice=N->Choices[Index];
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Choice.RequiredWorldFlag.IsNone() && (!Save || Save->GetSnapshot().WorldValues.FindRef(Choice.RequiredWorldFlag)==0))return false;
    return Choice.NextNodeId.IsNone() || ActiveDialogue->HasNode(Choice.NextNodeId);
}
bool UMythNarrativeSubsystem::Choose(int32 Index)
{
    if(!CanChoose(Index))return false;
    const FMythDialogueChoice Choice=ActiveDialogue->FindNode(ActiveNodeId)->Choices[Index];
    if(!Choice.SetWorldFlag.IsNone())
    {
        UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
        if(!Save)return false;
        FMythCampaignSnapshot S=Save->GetSnapshot();
        S.WorldValues.Add(Choice.SetWorldFlag,1);
        if(!Save->SetSnapshot(S))return false;
    }
    if(Choice.NextNodeId.IsNone()){StopDialogue();return true;}
    ActiveNodeId=Choice.NextNodeId;
    OnNodeEntered.Broadcast(CurrentDialogueId(),ActiveNodeId);
    return true;
}
void UMythNarrativeSubsystem::StopDialogue()
{
    if(!ActiveDialogue)return;
    const FName Id=CurrentDialogueId();const FName Node=ActiveNodeId;
    ActiveDialogue=nullptr;ActiveNodeId=NAME_None;
    OnDialogueFinished.Broadcast(Id,Node);
}
