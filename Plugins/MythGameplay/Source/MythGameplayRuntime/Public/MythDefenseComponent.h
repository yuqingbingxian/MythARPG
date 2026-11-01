#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MythDefenseComponent.generated.h"
class AMythCombatCharacter;

UENUM(BlueprintType)
enum class EMythDefenseOutcome : uint8 { None, Guarded, Parried };

/** Incoming attack resolution. Only combat damage pipeline can apply this outcome. */
USTRUCT(BlueprintType)
struct MYTHGAMEPLAYRUNTIME_API FMythDefenseResolution
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) EMythDefenseOutcome Outcome=EMythDefenseOutcome::None;
    UPROPERTY(BlueprintReadOnly) float HealthMultiplier=1.f;
    UPROPERTY(BlueprintReadOnly) float PoiseMultiplier=1.f;
    UPROPERTY(BlueprintReadOnly) float StaminaSpent=0.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMythDefenseEvent, EMythDefenseOutcome, Result);

/** Guard/parry/guard-break state with explicitly owned tags and timers. */
UCLASS(ClassGroup=(Myth),meta=(BlueprintSpawnableComponent))
class MYTHGAMEPLAYRUNTIME_API UMythDefenseComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UMythDefenseComponent();
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    UPROPERTY(BlueprintAssignable,Category="Myth|Defense") FMythDefenseEvent OnDefenseResolved;
    UFUNCTION(BlueprintCallable,Category="Myth|Defense") bool BeginGuard();
    UFUNCTION(BlueprintCallable,Category="Myth|Defense") void EndGuard();
    UFUNCTION(BlueprintCallable,Category="Myth|Defense") bool OpenParryWindow();
    UFUNCTION(BlueprintCallable,Category="Myth|Defense") void CloseParryWindow();
    UFUNCTION(BlueprintCallable,Category="Myth|Defense") void ResetDefense();
    UFUNCTION(BlueprintPure,Category="Myth|Defense") bool IsGuarding() const {return bGuarding;}
    UFUNCTION(BlueprintPure,Category="Myth|Defense") bool IsParrying() const {return bParryActive;}
    FMythDefenseResolution ResolveIncoming(const AMythCombatCharacter* Attacker,float RawDamage,float RawPoise);
protected:
    UPROPERTY(EditDefaultsOnly,Category="Myth|Defense",meta=(ClampMin="0.0",ClampMax="1.0")) float GuardHealthMultiplier=0.25f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Defense",meta=(ClampMin="0.0",ClampMax="1.0")) float GuardPoiseMultiplier=0.35f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Defense",meta=(ClampMin="0.0")) float GuardStaminaPerDamage=0.45f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Defense",meta=(ClampMin="0.05")) float ParryWindowSeconds=0.18f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Defense",meta=(ClampMin="0.0",ClampMax="1.0")) float FrontArcDotMin=0.10f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Defense",meta=(ClampMin="0.05")) float ParryCooldownSeconds=0.75f;
private:
    bool bGuarding=false;
    bool bParryActive=false;
    FTimerHandle ParryTimer;
    double LastParryTime=-1000.0;
    void ToggleTag(const TCHAR* Name,bool bEnabled);
};
