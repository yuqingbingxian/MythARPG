#include "MythBossCharacter.h"
#include "MythBossPhaseDefinition.h"
#include "MythSaveSubsystem.h"
#include "MythEventSubsystem.h"
#include "Engine/GameInstance.h"

AMythBossCharacter::AMythBossCharacter()
{
    DefaultMeleeDamage = 30.f;
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
    if (UMythSaveSubsystem* Save = GetGameInstance()->GetSubsystem<UMythSaveSubsystem>())
    {
        Save->RecordBossDefeat(BossId);
    }
    if (UMythEventSubsystem* Events = GetGameInstance()->GetSubsystem<UMythEventSubsystem>())
    {
        Events->PublishBossDefeated(BossId, ChapterId);
    }
}
