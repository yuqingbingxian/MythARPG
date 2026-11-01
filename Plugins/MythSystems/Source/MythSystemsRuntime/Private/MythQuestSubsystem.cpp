#include "MythQuestSubsystem.h"
#include "MythQuestDefinition.h"
#include "MythSaveSubsystem.h"
#include "MythEventSubsystem.h"
#include "Engine/GameInstance.h"
void UMythQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UMythEventSubsystem>();
    Super::Initialize(Collection);
    if(UMythEventSubsystem* Events=GetGameInstance()->GetSubsystem<UMythEventSubsystem>())
        Events->OnBossDefeated.AddUObject(this,&UMythQuestSubsystem::HandleBossDefeated);
}
void UMythQuestSubsystem::Deinitialize()
{
    if(UMythEventSubsystem* Events=GetGameInstance()->GetSubsystem<UMythEventSubsystem>())
        Events->OnBossDefeated.RemoveAll(this);
    Super::Deinitialize();
}
void UMythQuestSubsystem::HandleBossDefeated(FName BossId,FName ChapterId)
{
    ReportGameplayEvent(TEXT("BossDefeated"),BossId,1);
}
void UMythQuestSubsystem::RegisterCatalog(UMythQuestCatalog* Catalog){QuestCatalog=Catalog;}
FName UMythQuestSubsystem::GetCurrentStage(FName QuestId) const
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return NAME_None;
    const FMythCampaignSnapshot Snapshot=Save->GetSnapshot();
    const FMythQuestRecord* State=Snapshot.Quests.FindByPredicate(
        [QuestId](const FMythQuestRecord& R){return R.QuestId==QuestId;});
    return State && State->Status==1?State->StageId:NAME_None;
}
bool UMythQuestSubsystem::StartQuest(FName Id)
{
    if(!QuestCatalog || Id.IsNone())return false;
    const UMythQuestDefinition* Def=QuestCatalog->FindQuest(Id);
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Def || !Save || !Def->FindStage(Def->InitialStageId))return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if(S.Quests.ContainsByPredicate([Id](const FMythQuestRecord& R){return R.QuestId==Id;}))return false;
    for(FName Flag:Def->RequiredWorldFlags)
        if(S.WorldValues.FindRef(Flag)==0)return false;
    FMythQuestRecord Record;Record.QuestId=Id;Record.StageId=Def->InitialStageId;Record.Status=1;
    S.Quests.Add(Record);
    if(!Save->SetSnapshot(S))return false;
    OnQuestAdvanced.Broadcast(Id,Record.StageId);
    return true;
}
bool UMythQuestSubsystem::ReportGameplayEvent(FName EventName,FName TargetId,int32 Amount)
{
    if(!QuestCatalog || EventName.IsNone() || Amount<=0)return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    TArray<TPair<FName,FName>> Advances;
    TArray<TPair<FName,FName>> Completions;
    bool bModified=false;
    for(FMythQuestRecord& R:S.Quests)
    {
        if(R.Status!=1)continue;
        const UMythQuestDefinition* Def=QuestCatalog->FindQuest(R.QuestId);
        const FMythQuestStage* Stage=Def?Def->FindStage(R.StageId):nullptr;
        if(!Stage)continue;
        bool bTouched=false;
        for(const FMythQuestObjective& Obj:Stage->Objectives)
        {
            if(Obj.EventName!=EventName || (!Obj.TargetId.IsNone() && Obj.TargetId!=TargetId))continue;
            const int32 Old=R.ObjectiveCounts.FindRef(Obj.ObjectiveId);
            const int32 New=FMath::Min(Obj.RequiredCount,Old+FMath::Min(Amount,Obj.RequiredCount));
            if(New!=Old){R.ObjectiveCounts.Add(Obj.ObjectiveId,New);bTouched=true;}
        }
        if(!bTouched)continue;
        bModified=true;
        bool bComplete=true;
        for(const FMythQuestObjective& Obj:Stage->Objectives)
            if(R.ObjectiveCounts.FindRef(Obj.ObjectiveId)<Obj.RequiredCount){bComplete=false;break;}
        if(!bComplete)continue;
        if(!Stage->WorldFlagOnCompletion.IsNone())S.WorldValues.Add(Stage->WorldFlagOnCompletion,1);
        R.ObjectiveCounts.Reset();
        if(Stage->NextStageId.IsNone())
        {
            R.Status=2;Completions.Add(TPair<FName,FName>(R.QuestId,R.StageId));
        }
        else
        {
            R.StageId=Stage->NextStageId;
            Advances.Add(TPair<FName,FName>(R.QuestId,R.StageId));
        }
    }
    if(!bModified || !Save->SetSnapshot(S))return false;
    for(const auto& Pair:Advances)OnQuestAdvanced.Broadcast(Pair.Key,Pair.Value);
    for(const auto& Pair:Completions)OnQuestCompleted.Broadcast(Pair.Key,Pair.Value);
    return true;
}
