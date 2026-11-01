#include "MythBossCharacter.h"
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
    OnPhaseEntered(PhaseIndex);
}

void AMythBossCharacter::HandleHealthChanged(float NewHealth)
{
    Super::HandleHealthChanged(NewHealth);
    if (bDead || GetMaxHealth() <= 0.f) return;
    const float Ratio = NewHealth / GetMaxHealth();
    const int32 DesiredPhase = Ratio <= 0.33f ? 3 : Ratio <= 0.66f ? 2 : 1;
    if (DesiredPhase > PhaseIndex && NewHealth > 0.f)
    {
        PhaseIndex = DesiredPhase;
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
