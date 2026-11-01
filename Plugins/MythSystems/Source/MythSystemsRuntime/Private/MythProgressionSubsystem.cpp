#include "MythProgressionSubsystem.h"
#include "MythSaveSubsystem.h"
#include "MythSkillTreeDefinition.h"
#include "Engine/GameInstance.h"
void UMythProgressionSubsystem::RegisterRules(UMythProgressionRules* R){Rules=R;}
void UMythProgressionSubsystem::RegisterSkillTree(UMythSkillTreeDefinition* T){SkillTree=T;}
bool UMythProgressionSubsystem::TryAwardToSnapshot(FMythCampaignSnapshot& S,int32 Amount)const
{
    if(!Rules || Amount<=0 || Amount>MAX_int32-S.Experience)return false;
    FMythCampaignSnapshot Candidate=S;
    Candidate.Experience+=Amount;
    while(Candidate.PlayerLevel>=1 && Candidate.PlayerLevel<=Rules->ExperiencePerLevel.Num())
    {
        const int32 Required=Rules->ExperiencePerLevel[Candidate.PlayerLevel-1];
        if(Required<=0 || Candidate.Experience<Required)break;
        if(Candidate.SkillPoints>MAX_int32-FMath::Max(0,Rules->SkillPointsPerLevel))return false;
        Candidate.Experience-=Required;++Candidate.PlayerLevel;
        Candidate.SkillPoints+=FMath::Max(0,Rules->SkillPointsPerLevel);
    }
    S=MoveTemp(Candidate);
    return true;
}
bool UMythProgressionSubsystem::AwardExperience(int32 Amount)
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    return TryAwardToSnapshot(S,Amount)&&Save->SetSnapshot(S);
}
bool UMythProgressionSubsystem::UnlockSkill(FName Id,int32 Cost)
{
    if(Id.IsNone() || Cost<0 || !SkillTree)return false;
    const FMythSkillNode* Node=SkillTree->FindSkill(Id);
    if(!Node || Node->SkillPointCost!=Cost)return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if(S.UnlockedSkills.Contains(Id) || S.SkillPoints<Cost)return false;
    for(FName Required:Node->Prerequisites)if(!S.UnlockedSkills.Contains(Required))return false;
    S.SkillPoints-=Cost;
    S.UnlockedSkills.Add(Id);
    return Save->SetSnapshot(S);
}
bool UMythProgressionSubsystem::IsSkillUnlocked(FName Id) const
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    return Save && Save->GetSnapshot().UnlockedSkills.Contains(Id);
}
