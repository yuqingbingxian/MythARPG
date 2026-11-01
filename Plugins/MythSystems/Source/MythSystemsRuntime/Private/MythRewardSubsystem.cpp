#include "MythRewardSubsystem.h"
#include "MythRewardDefinition.h"
#include "MythSaveSubsystem.h"
#include "MythInventorySubsystem.h"
#include "MythProgressionSubsystem.h"
#include "Engine/GameInstance.h"
bool UMythRewardSubsystem::TryGrantToSnapshot(FMythCampaignSnapshot& S,const UMythRewardDefinition* Reward)const
{
    if(!Reward || Reward->RewardId.IsNone() || Reward->Experience<0 || Reward->SkillPoints<0)return false;
    if(S.AppliedTransactionIds.Contains(Reward->RewardId))return true;
    UMythInventorySubsystem* Inventory=GetGameInstance()->GetSubsystem<UMythInventorySubsystem>();
    UMythProgressionSubsystem* Progress=GetGameInstance()->GetSubsystem<UMythProgressionSubsystem>();
    // Work on a local copy so an invalid reward cannot partly modify its caller.
    FMythCampaignSnapshot Candidate=S;
    for(const FMythItemStack& Item:Reward->Items)
    {
        if(!Inventory || !Inventory->TryAddToSnapshot(Candidate,Item.ItemId,Item.Count))return false;
    }
    for(const FMythCurrencyReward& Money:Reward->Currency)
    {
        if(Money.CurrencyId.IsNone() || Money.Amount<0)return false;
        const int64 Current=Candidate.Wallet.FindRef(Money.CurrencyId);
        if(Current<0 || Money.Amount>MAX_int64-Current)return false;
        Candidate.Wallet.Add(Money.CurrencyId,Current+Money.Amount);
    }
    if(Reward->Experience>0 && (!Progress || !Progress->TryAwardToSnapshot(Candidate,Reward->Experience)))return false;
    if(Reward->SkillPoints>MAX_int32-Candidate.SkillPoints)return false;
    Candidate.SkillPoints+=Reward->SkillPoints;
    for(FName Flag:Reward->SetWorldFlags)if(!Flag.IsNone())Candidate.WorldValues.Add(Flag,1);
    Candidate.AppliedTransactionIds.Add(Reward->RewardId);
    S=MoveTemp(Candidate);
    return true;
}
bool UMythRewardSubsystem::GrantReward(const UMythRewardDefinition* Reward)
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if(!TryGrantToSnapshot(S,Reward) || !Save->SetSnapshot(S))return false;
    if(UMythInventorySubsystem* Inventory=GetGameInstance()->GetSubsystem<UMythInventorySubsystem>())Inventory->NotifyInventoryChanged();
    return true;
}
