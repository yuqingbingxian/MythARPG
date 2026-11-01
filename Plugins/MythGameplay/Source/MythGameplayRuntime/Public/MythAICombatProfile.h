#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythAICombatProfile.generated.h"

/** Attack selection is authored data, not hard-coded per enemy subclass. */
USTRUCT(BlueprintType)
struct FMythAIAttackOption
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName AttackId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="0")) float MinDistance=0.f;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="0")) float MaxDistance=210.f;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="0")) float Weight=1.f;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="0")) float Cooldown=1.4f;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) bool bHeavyAttack=false;
};
UCLASS(BlueprintType)
class MYTHGAMEPLAYRUNTIME_API UMythAICombatProfile : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythAIAttackOption> Attacks;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin="0")) float ForgetTargetSeconds=4.f;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin="100")) float HomeLeashDistance=2000.f;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin="0")) float PreferredFightDistance=150.f;
    /** Pure selection; randomness supplied by seeded stream for repeatable automated tests. */
    const FMythAIAttackOption* ChooseAttack(float Distance,const TMap<FName,double>& LastUsed,
        double Now, FRandomStream& Random) const;
};
