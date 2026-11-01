#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MythCombatDirectorSubsystem.generated.h"
USTRUCT()
struct FMythAttackTicket
{
    GENERATED_BODY()
    TWeakObjectPtr<AActor> Attacker;
    TWeakObjectPtr<AActor> Target;
    double ExpiresAt=0;
};
/** Limits simultaneous high-pressure enemies without managing actual Ability execution. */
UCLASS()
class MYTHGAMEPLAYRUNTIME_API UMythCombatDirectorSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) bool TryReserveAttack(AActor* Attacker,AActor* Target,float HoldSeconds=2.f);
    UFUNCTION(BlueprintCallable) void ReleaseAttack(AActor* Attacker);
private:
    TArray<FMythAttackTicket> Tickets;
    int32 MaxAttackersPerTarget=2;
    void RemoveExpired();
};
