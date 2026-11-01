#include "MythEventSubsystem.h"
void UMythEventSubsystem::PublishBossDefeated(FName BossId, FName ChapterId)
{
    if (!BossId.IsNone())
    {
        OnBossDefeated.Broadcast(BossId, ChapterId);
    }
}
