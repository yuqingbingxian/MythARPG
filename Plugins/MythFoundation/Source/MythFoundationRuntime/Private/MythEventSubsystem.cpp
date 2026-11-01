#include "MythEventSubsystem.h"
#include "MythSaveSubsystem.h"
#include "Engine/GameInstance.h"

void UMythEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UMythSaveSubsystem>();
    Super::Initialize(Collection);
    // Bootstrap replays only after quest systems have subscribed and registered their catalogs.
}

bool UMythEventSubsystem::PublishSignal(const FMythGameplaySignal& Signal)
{
    if(Signal.EventName.IsNone() || Signal.Count<=0 || !GetGameInstance())return false;
    if(Signal.StableReceiptId.IsNone())
    {
        // Transient signals (combat feedback) intentionally don't use the durable outbox.
        OnSignal.Broadcast(Signal);OnSignalBP.Broadcast(Signal);
        return true;
    }
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if(S.AppliedTransactionIds.Contains(Signal.StableReceiptId) ||
       S.PendingSignals.ContainsByPredicate([&Signal](const FMythQueuedSignal& E)
            {return E.ReceiptId==Signal.StableReceiptId;}))return false;
    FMythQueuedSignal Q;
    Q.ReceiptId=Signal.StableReceiptId;Q.EventName=Signal.EventName;
    Q.TargetId=Signal.TargetId;Q.ChapterId=Signal.ChapterId;Q.Count=Signal.Count;
    S.PendingSignals.Add(Q);
    if(!Save->SetSnapshot(S,true))return false;
    DeliverPending();
    return true;
}

void UMythEventSubsystem::DeliverPending()
{
    if(bDispatching || !GetGameInstance())return;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save || Save->IsSaveInProgress())return;
    TGuardValue<bool> Guard(bDispatching,true);
    // Hard cap prevents an infinite self-producing sequence from blocking startup.
    for(int32 Index=0;Index<512;Index++)
    {
        FMythCampaignSnapshot S=Save->GetSnapshot();
        if(S.PendingSignals.IsEmpty())break;
        const FMythQueuedSignal Item=S.PendingSignals[0];
        if(!S.AppliedTransactionIds.Contains(Item.ReceiptId))
        {
            FMythGameplaySignal Delivered;
            Delivered.StableReceiptId=Item.ReceiptId;
            Delivered.EventName=Item.EventName;Delivered.TargetId=Item.TargetId;
            Delivered.ChapterId=Item.ChapterId;Delivered.Count=Item.Count;
            OnSignal.Broadcast(Delivered);
            OnSignalBP.Broadcast(Delivered);
            if(Delivered.EventName==TEXT("BossDefeated"))
                OnBossDefeated.Broadcast(Delivered.TargetId,Delivered.ChapterId);
        }
        // A receiver may have updated the campaign. Always load the latest snapshot before ACK.
        S=Save->GetSnapshot();
        S.PendingSignals.RemoveAll([&Item](const FMythQueuedSignal& E){return E.ReceiptId==Item.ReceiptId;});
        S.AppliedTransactionIds.Add(Item.ReceiptId);
        if(!Save->SetSnapshot(S,true))break;
    }
}

void UMythEventSubsystem::PublishBossDefeated(FName BossId,FName ChapterId)
{
    if(BossId.IsNone())return;
    FMythGameplaySignal Signal;
    Signal.EventName=TEXT("BossDefeated");Signal.TargetId=BossId;Signal.ChapterId=ChapterId;
    Signal.StableReceiptId=FName(*(FString(TEXT("Event.BossDefeated."))+ChapterId.ToString()+TEXT(".")+BossId.ToString()));
    PublishSignal(Signal);
}
