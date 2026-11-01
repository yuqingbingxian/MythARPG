#include "MythBossPhaseDefinition.h"
bool UMythBossPhaseDefinition::ValidatePhases(FString& Error)const
{
    if(Phases.IsEmpty()){Error=TEXT("No phases defined");return false;}
    float Previous=1.01f;
    TSet<FName> IDs;
    for(const FMythBossPhase& Phase:Phases)
    {
        if(Phase.PhaseId.IsNone() || IDs.Contains(Phase.PhaseId) ||
           Phase.EnterAtOrBelowHealthRatio<0.f || Phase.EnterAtOrBelowHealthRatio>1.f ||
           Phase.EnterAtOrBelowHealthRatio>=Previous)
        {Error=TEXT("Invalid phase ID or unordered thresholds");return false;}
        IDs.Add(Phase.PhaseId);Previous=Phase.EnterAtOrBelowHealthRatio;
    }
    return true;
}
int32 UMythBossPhaseDefinition::CalculatePhaseIndex(float HealthRatio,int32 CurrentIndex)const
{
    if(Phases.IsEmpty())return 0;
    const float Ratio=FMath::Clamp(HealthRatio,0.f,1.f);
    // Transition only to an adjacent phase. Explicit boss mechanism may opt into skipping.
    int32 Target=FMath::Clamp(CurrentIndex,0,Phases.Num()-1);
    if(Target+1<Phases.Num() && Ratio<=Phases[Target+1].EnterAtOrBelowHealthRatio)
    {
        Target++;
        if(Phases[Target].bAllowSkipPhase)
            while(Target+1<Phases.Num() && Ratio<=Phases[Target+1].EnterAtOrBelowHealthRatio)
                Target++;
    }
    return Target;
}
