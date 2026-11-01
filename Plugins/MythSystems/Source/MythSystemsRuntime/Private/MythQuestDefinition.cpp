#include "MythQuestDefinition.h"
const FMythQuestStage* UMythQuestDefinition::FindStage(FName Id) const
{
    return Stages.FindByPredicate([Id](const FMythQuestStage& Stage){return Stage.StageId==Id;});
}
FName UMythQuestDefinition::ResolveNextStage(const FMythQuestStage& Stage,const TMap<FName,int32>& WorldValues)const
{
    for(const FMythQuestBranch& Rule:Stage.ConditionalBranches)
        if(!Rule.RequiredWorldFlag.IsNone() && WorldValues.FindRef(Rule.RequiredWorldFlag)>=Rule.MinimumValue)
            return Rule.NextStageId;
    return Stage.NextStageId;
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
    {
        if(!Stage.NextStageId.IsNone() && !StageIds.Contains(Stage.NextStageId))
        { Error=TEXT("Missing next stage");return false; }
        if(Stage.NextStageId==Stage.StageId)
        {Error=TEXT("Direct self loop in quest stage");return false;}
        for(const FMythQuestBranch& Rule:Stage.ConditionalBranches)
            if(Rule.RequiredWorldFlag.IsNone() || Rule.MinimumValue<1 ||
               (!Rule.NextStageId.IsNone() && !StageIds.Contains(Rule.NextStageId)) ||
               Rule.NextStageId==Stage.StageId)
            {Error=TEXT("Invalid conditional stage branch");return false;}
    }
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
