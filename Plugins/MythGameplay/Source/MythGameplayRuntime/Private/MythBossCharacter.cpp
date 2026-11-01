#include "MythBossCharacter.h"
#include "MythBossPhaseDefinition.h"
#include "MythSaveSubsystem.h"
#include "MythEventSubsystem.h"
#include "Engine/GameInstance.h"

AMythBossCharacter::AMythBossCharacter()
{
    DefaultMeleeDamage = 30.f;
    bReportGenericEnemyDeath=false;
}

void AMythBossCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (UMythSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UMythSaveSubsystem>())
    {
        if (Save->IsBossDefeated(BossId))
        {
            Destroy();
            return;
        }
    }
    if(PhaseDefinition && !PhaseDefinition->Phases.IsEmpty())PhaseIndex=1;
    OnPhaseEntered(PhaseIndex);
}

void AMythBossCharacter::HandleHealthChanged(float NewHealth)
{
    Super::HandleHealthChanged(NewHealth);
    if (bDead || GetMaxHealth() <= 0.f) return;
    const float Ratio = NewHealth / GetMaxHealth();
    const int32 DesiredPhase = PhaseDefinition && !PhaseDefinition->Phases.IsEmpty()
        ? PhaseDefinition->CalculatePhaseIndex(Ratio,PhaseIndex-1)+1
        : (Ratio<=.33f?3:(Ratio<=.66f?2:1));
    if(DesiredPhase > PhaseIndex && NewHealth > 0.f)
    {
        PhaseIndex=DesiredPhase;
        if(PhaseDefinition && PhaseDefinition->Phases.IsValidIndex(PhaseIndex-1))
        {
            const FMythBossPhase& P=PhaseDefinition->Phases[PhaseIndex-1];
            OnBossPhaseCue(P.PhaseId,P.CinematicCue);
        }
        OnPhaseEntered(PhaseIndex);
    }
}

void AMythBossCharacter::HandleDeath()
{
    if (bDead) return;
    Super::HandleDeath();
    UMythSaveSubsystem* Save=GetGameInstance()?GetGameInstance()->GetSubsystem<UMythSaveSubsystem>():nullptr;
    if(!Save || BossId.IsNone())return;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if(!S.DefeatedBossIds.Contains(BossId))
    {
        S.DefeatedBossIds.Add(BossId);
        FMythQueuedSignal Event;
        Event.ReceiptId=FName(*(FString(TEXT("Event.BossDefeated."))+ChapterId.ToString()+TEXT(".")+BossId.ToString()));
        Event.EventName=TEXT("BossDefeated");Event.ChapterId=ChapterId;
        Event.TargetId=BossId;Event.Count=1;
        S.PendingSignals.Add(Event);
        // Commit defeat + event in one durable snapshot BEFORE any quest listeners run.
        if(!Save->SetSnapshot(S,true))
        {
            UE_LOG(LogTemp,Error,TEXT("Unable to persist boss defeat %s"),*BossId.ToString());
            return;
        }
    }
    if(UMythEventSubsystem* Events=GetGameInstance()->GetSubsystem<UMythEventSubsystem>())
        Events->DeliverPending();
}
