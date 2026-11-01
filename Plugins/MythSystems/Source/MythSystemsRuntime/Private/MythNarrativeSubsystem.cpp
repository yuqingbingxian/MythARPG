#include "MythNarrativeSubsystem.h"
#include "MythDialogueDefinition.h"
#include "MythSaveSubsystem.h"
#include "MythEventSubsystem.h"
#include "MythNarrativeCommandSubsystem.h"
#include "MythNarrativeCommandDefinition.h"
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
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    UMythNarrativeCommandSubsystem* Commands=GetGameInstance()->GetSubsystem<UMythNarrativeCommandSubsystem>();
    const UMythNarrativeCommandDefinition* Sequence=nullptr;
    if(!Choice.ActionSequence.IsNull())
    {
        Sequence=Choice.ActionSequence.LoadSynchronous();
        if(!Sequence || !Commands || !Save)return false;
    }
    if(Sequence || !Choice.SetWorldFlag.IsNone())
    {
        if(!Save)return false;
        FMythCampaignSnapshot Candidate=Save->GetSnapshot();
        if(Sequence && !Commands->ApplyToSnapshot(Sequence,Candidate))return false;
        if(!Choice.SetWorldFlag.IsNone())Candidate.WorldValues.Add(Choice.SetWorldFlag,1);
        if(!Save->SetSnapshot(Candidate))return false;
        if(Sequence)Commands->PublishEvents(Sequence);
    }
    if(Choice.NextNodeId.IsNone())
    {
        const FName CompletedId=CurrentDialogueId();
        StopDialogue();
        if(UMythEventSubsystem* Events=GetGameInstance()->GetSubsystem<UMythEventSubsystem>())
        {
            FMythGameplaySignal Signal;Signal.EventName=TEXT("DialogueCompleted");Signal.TargetId=CompletedId;
            Events->PublishSignal(Signal);
        }
        return true;
    }
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
