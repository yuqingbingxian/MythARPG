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
    for(int32 CommandIndex=0;CommandIndex<Def->Commands.Num();++CommandIndex)
    {
        const FMythNarrativeCommand& C=Def->Commands[CommandIndex];
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
        case EMythNarrativeOpcode::EmitGameplaySignal:
        {
            FMythQueuedSignal Pending;
            Pending.ReceiptId=FName(*FString::Printf(TEXT("Narrative.%s.%d"),*Def->ReceiptId.ToString(),CommandIndex));
            Pending.EventName=C.TargetId;Pending.Count=FMath::Max(1,C.IntValue);
            S.PendingSignals.Add(Pending);
            break;
        }
        default:return false;
        }
    }
    S.AppliedTransactionIds.Add(Def->ReceiptId);
    return true;
}
void UMythNarrativeCommandSubsystem::PublishEvents(const UMythNarrativeCommandDefinition* Def)const
{
    // Compatibility wrapper. The outbox was already committed by RunSequence.
    if(UGameInstance* GI=GetGameInstance())
        if(UMythEventSubsystem* Bus=GI->GetSubsystem<UMythEventSubsystem>())Bus->DeliverPending();
}
bool UMythNarrativeCommandSubsystem::RunSequence(UMythNarrativeCommandDefinition* Def)
{
    UGameInstance* GI=GetGameInstance();
    UMythSaveSubsystem* Save=GI?GI->GetSubsystem<UMythSaveSubsystem>():nullptr;
    if(!Def || !Save)return false;
    FMythCampaignSnapshot Candidate=Save->GetSnapshot();
    if(!ApplyToSnapshot(Def,Candidate) || !Save->SetSnapshot(Candidate,true))return false;
    PublishEvents(Def);
    return true;
}
