#include "MythEventSubsystem.h"
#include "MythSaveSubsystem.h"
#include "Engine/GameInstance.h"
bool UMythEventSubsystem::PublishSignal(const FMythGameplaySignal& Signal)
{
    if(Signal.EventName.IsNone() || Signal.Count<=0)return false;
    if(!Signal.StableReceiptId.IsNone())
    {
        UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
        if(!Save)return false;
        FMythCampaignSnapshot S=Save->GetSnapshot();
        if(S.AppliedTransactionIds.Contains(Signal.StableReceiptId))return false;
        S.AppliedTransactionIds.Add(Signal.StableReceiptId);
        if(!Save->SetSnapshot(S))return false;
    }
    OnSignal.Broadcast(Signal);
    OnSignalBP.Broadcast(Signal);
    return true;
}
void UMythEventSubsystem::PublishBossDefeated(FName BossId,FName ChapterId)
{
    if(BossId.IsNone())return;
    FMythGameplaySignal Signal;
    Signal.EventName=TEXT("BossDefeated");Signal.TargetId=BossId;Signal.ChapterId=ChapterId;
    Signal.StableReceiptId=FName(*(FString(TEXT("Event.BossDefeated."))+BossId.ToString()));
    if(PublishSignal(Signal))OnBossDefeated.Broadcast(BossId,ChapterId);
}
