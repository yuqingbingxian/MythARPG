#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythEventSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMythBossDefeated, FName /*BossId*/, FName /*ChapterId*/);

/** Fact notifications only. Consumers must decide and persist their own changes. */
UCLASS()
class MYTHFOUNDATIONRUNTIME_API UMythEventSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    FOnMythBossDefeated OnBossDefeated;
    void PublishBossDefeated(FName BossId, FName ChapterId);
};
