#include "MythQuestDefinition.h"
const FMythQuestStage* UMythQuestDefinition::FindStage(FName Id) const
{
    return Stages.FindByPredicate([Id](const FMythQuestStage& Stage){return Stage.StageId==Id;});
}
bool UMythQuestDefinition::ValidateDefinition(FString& Error) const
{
    if(QuestId.IsNone() || InitialStageId.IsNone() || !FindStage(InitialStageId))
    { Error=TEXT("Missing QuestId or initial stage");return false; }
    TSet<FName> StageIds;
    for(const FMythQuestStage& Stage:Stages)
    {
        if(Stage.StageId.IsNone() || StageIds.Contains(Stage.StageId))
        { Error=TEXT("Duplicate/empty stage id");return false; }
        StageIds.Add(Stage.StageId);
        TSet<FName> ObjectiveIds;
        for(const FMythQuestObjective& Obj:Stage.Objectives)
        {
            if(Obj.ObjectiveId.IsNone() || Obj.EventName.IsNone() || Obj.RequiredCount<1 || ObjectiveIds.Contains(Obj.ObjectiveId))
            { Error=TEXT("Invalid or duplicate objective");return false; }
            ObjectiveIds.Add(Obj.ObjectiveId);
        }
    }
    for(const FMythQuestStage& Stage:Stages)
        if(!Stage.NextStageId.IsNone() && !StageIds.Contains(Stage.NextStageId))
        { Error=TEXT("Missing next stage");return false; }
    return true;
}
UMythQuestDefinition* UMythQuestCatalog::FindQuest(FName Id) const
{
    for(const TSoftObjectPtr<UMythQuestDefinition>& Ref:Quests)
    {
        UMythQuestDefinition* D=Ref.LoadSynchronous();
        if(D && D->QuestId==Id)return D;
    }
    return nullptr;
}
