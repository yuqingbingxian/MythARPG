#include "MythProgressionSubsystem.h"
#include "MythSaveSubsystem.h"
#include "MythSkillTreeDefinition.h"
#include "Engine/GameInstance.h"
void UMythProgressionSubsystem::RegisterRules(UMythProgressionRules* R){Rules=R;}
void UMythProgressionSubsystem::RegisterSkillTree(UMythSkillTreeDefinition* T){SkillTree=T;}
bool UMythProgressionSubsystem::AwardExperience(int32 Amount)
{
    if(!Rules || Amount<=0)return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if(Amount>MAX_int32-S.Experience)return false;
    S.Experience+=Amount;
    while(S.PlayerLevel>=1 && S.PlayerLevel<=Rules->ExperiencePerLevel.Num())
    {
        const int32 Required=Rules->ExperiencePerLevel[S.PlayerLevel-1];
        if(Required<=0 || S.Experience<Required)break;
        S.Experience-=Required;
        ++S.PlayerLevel;
        S.SkillPoints+=FMath::Max(0,Rules->SkillPointsPerLevel);
    }
    return Save->SetSnapshot(S);
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
