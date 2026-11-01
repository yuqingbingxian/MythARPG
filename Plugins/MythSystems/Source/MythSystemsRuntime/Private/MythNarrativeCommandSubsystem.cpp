#include "MythNarrativeCommandSubsystem.h"
#include "MythNarrativeCommandDefinition.h"
#include "MythRewardSubsystem.h"
#include "MythRewardDefinition.h"
#include "MythSaveSubsystem.h"
#include "MythEventSubsystem.h"
#include "Engine/GameInstance.h"

bool UMythNarrativeCommandSubsystem::ApplyToSnapshot(const UMythNarrativeCommandDefinition* Def,
    FMythCampaignSnapshot& S)const
{
    FString Error;
    if(!Def || !Def->Validate(Error) || S.AppliedTransactionIds.Contains(Def->ReceiptId))return false;
    if(!Def->RequiredWorldFlag.IsNone() && S.WorldValues.FindRef(Def->RequiredWorldFlag)==0)return false;
    for(const FMythNarrativeCommand& C:Def->Commands)
    {
        switch(C.Opcode)
        {
        case EMythNarrativeOpcode::SetWorldValue:
            S.WorldValues.Add(C.TargetId,C.IntValue);break;
        case EMythNarrativeOpcode::AddWorldValue:
        {
            const int64 Sum=int64(S.WorldValues.FindRef(C.TargetId))+int64(C.IntValue);
            if(Sum<MIN_int32 || Sum>MAX_int32)return false;
            S.WorldValues.Add(C.TargetId,static_cast<int32>(Sum));break;
        }
        case EMythNarrativeOpcode::AddCurrency:
        {
            const int64 Current=S.Wallet.FindRef(C.TargetId);
            if(C.IntValue<0 || Current<0 || Current>MAX_int64-C.IntValue)return false;
            S.Wallet.Add(C.TargetId,Current+C.IntValue);break;
        }
        case EMythNarrativeOpcode::UnlockChapter:
            S.UnlockedChapterIds.Add(C.TargetId);break;
        case EMythNarrativeOpcode::GrantReward:
        {
            const UMythRewardDefinition* Reward=C.Reward.LoadSynchronous();
            UGameInstance* GI=GetGameInstance();
            UMythRewardSubsystem* Rewards=GI?GI->GetSubsystem<UMythRewardSubsystem>():nullptr;
            if(!Rewards || !Reward || !Rewards->TryGrantToSnapshot(S,Reward))return false;
            break;
        }
        case EMythNarrativeOpcode::EmitGameplaySignal:break; // Publish only after successful commit.
        default:return false;
        }
    }
    S.AppliedTransactionIds.Add(Def->ReceiptId);
    return true;
}
void UMythNarrativeCommandSubsystem::PublishEvents(const UMythNarrativeCommandDefinition* Def)const
{
    UGameInstance* GI=GetGameInstance();
    UMythEventSubsystem* Bus=GI?GI->GetSubsystem<UMythEventSubsystem>():nullptr;
    if(!Bus || !Def)return;
    for(const FMythNarrativeCommand& C:Def->Commands)
        if(C.Opcode==EMythNarrativeOpcode::EmitGameplaySignal)
        {
            FMythGameplaySignal Signal;Signal.EventName=C.TargetId;Signal.Count=FMath::Max(1,C.IntValue);
            Bus->PublishSignal(Signal);
        }
}
bool UMythNarrativeCommandSubsystem::RunSequence(UMythNarrativeCommandDefinition* Def)
{
    UGameInstance* GI=GetGameInstance();
    UMythSaveSubsystem* Save=GI?GI->GetSubsystem<UMythSaveSubsystem>():nullptr;
    if(!Def || !Save)return false;
    FMythCampaignSnapshot Candidate=Save->GetSnapshot();
    if(!ApplyToSnapshot(Def,Candidate) || !Save->SetSnapshot(Candidate))return false;
    PublishEvents(Def);
    return true;
}
