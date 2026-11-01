#include "MythSkillTreeDefinition.h"
const FMythSkillNode* UMythSkillTreeDefinition::FindSkill(FName Id)const
{return Nodes.FindByPredicate([Id](const FMythSkillNode& N){return N.SkillId==Id;});}
bool UMythSkillTreeDefinition::ValidateTree(FString& Error)const
{
    TSet<FName> Seen;
    for(const FMythSkillNode& N:Nodes)
    {
        if(N.SkillId.IsNone() || N.SkillPointCost<0 || Seen.Contains(N.SkillId))
        {Error=TEXT("Duplicate/invalid skill node");return false;}
        Seen.Add(N.SkillId);
    }
    for(const FMythSkillNode& N:Nodes)
    {
        if(N.Prerequisites.Contains(N.SkillId))
        {Error=TEXT("Self dependency");return false;}
        for(FName Required:N.Prerequisites)if(!Seen.Contains(Required))
        {Error=TEXT("Unknown prerequisite");return false;}
    }
    // Reachability / cycle validation using deterministic Kahn processing.
    TSet<FName> Visited;
    for(int32 Pass=0;Pass<Nodes.Num();++Pass)
        for(const FMythSkillNode& N:Nodes)
        {
            if(Visited.Contains(N.SkillId))continue;
            bool bReady=true;
            for(FName Req:N.Prerequisites)if(!Visited.Contains(Req)){bReady=false;break;}
            if(bReady)Visited.Add(N.SkillId);
        }
    if(Visited.Num()!=Nodes.Num()){Error=TEXT("Circular prerequisites");return false;}
    return true;
}
