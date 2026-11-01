#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MythComboGraph.h"
#include "MythActionComponent.generated.h"
class UMythMeleeAbility;
class UMythDodgeAbility;
class UMythWeaponTraceComponent;
class UAnimMontage;

UENUM(BlueprintType)
enum class EMythActionState : uint8 { Idle, Attacking, Dodging };

/** Owns the action state machine, combo sequence, timers and input buffering.
 * GAS abilities activate this component; it never bypasses ASC for action activation. */
UCLASS(ClassGroup=(Myth),meta=(BlueprintSpawnableComponent))
class MYTHGAMEPLAYRUNTIME_API UMythActionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UMythActionComponent();
    virtual void BeginPlay() override;
    UFUNCTION(BlueprintCallable,Category="Myth|Action") bool RequestAttack(EMythAttackInput Input);
    UFUNCTION(BlueprintCallable,Category="Myth|Action") bool RequestDodge();
    UFUNCTION(BlueprintPure,Category="Myth|Action") EMythActionState GetActionState() const {return State;}
    UFUNCTION(BlueprintPure,Category="Myth|Action") FName GetCurrentNodeId() const {return CurrentNode.NodeId;}
    UFUNCTION(BlueprintPure,Category="Myth|Action") bool CanDodgeCancel() const {return State==EMythActionState::Attacking && bDodgeCancelWindow;}
    bool BeginAttack(UMythMeleeAbility* InAbility);
    void AbortAttack(UMythMeleeAbility* InAbility);
    bool BeginDodge(UMythDodgeAbility* InAbility);
    void AbortDodge(UMythDodgeAbility* InAbility);
    void OpenHitWindow();
    void CloseHitWindow();
    void OpenComboWindow();
    void CloseComboWindow();
    void OpenDodgeCancelWindow();
    void CloseDodgeCancelWindow();
    void OpenDodgeIFrames();
    void CloseDodgeIFrames();
    void ForceReset();
protected:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Myth|Action") TObjectPtr<UMythComboGraph> ComboGraph;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Action",meta=(ClampMin="0.05")) float InputBufferSeconds = 0.38f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Dodge",meta=(ClampMin="0.1")) float DodgeDuration = 0.5f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Dodge",meta=(ClampMin="0")) float DodgeStaminaCost = 18.f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Dodge",meta=(ClampMin="0")) float DodgeImpulse = 900.f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Dodge") TObjectPtr<UAnimMontage> DodgeMontage;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Dodge") bool bDodgeUseAnimationNotifies = false;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Dodge") float DodgeIFrameOpen = 0.07f;
    UPROPERTY(EditDefaultsOnly,Category="Myth|Dodge") float DodgeIFrameClose = 0.30f;
private:
    EMythActionState State = EMythActionState::Idle;
    UPROPERTY(Transient) TArray<FMythComboNode> FallbackNodes;
    FMythComboNode CurrentNode;
    UPROPERTY(Transient) TObjectPtr<UAnimMontage> ActiveMontage;
    TWeakObjectPtr<UMythMeleeAbility> AttackAbility;
    TWeakObjectPtr<UMythDodgeAbility> DodgeAbility;
    EMythAttackInput RequestedEntry = EMythAttackInput::Light;
    EMythAttackInput BufferedInput = EMythAttackInput::Light;
    bool bBuffered = false;
    double BufferedAt = 0.0;
    bool bComboWindow = false;
    bool bDodgeCancelWindow = false;
    bool bHitWindow = false;
    bool bIFrames = false;
    FTimerHandle HitOpenTimer,HitCloseTimer,ComboOpenTimer,ComboCloseTimer;
    FTimerHandle CancelOpenTimer,CancelCloseTimer,ActionEndTimer;
    FTimerHandle IFrameOpenTimer,IFrameCloseTimer;

    const FMythComboNode* FindEntry(EMythAttackInput Input) const;
    const FMythComboNode* FindNext(FName Id,EMythAttackInput Input) const;
    bool StartNode(const FMythComboNode& Node);
    bool TryAdvance(EMythAttackInput Input);
    void ConsumeBuffered();
    bool SpendStamina(float Amount);
    void EndAttackNaturally();
    void EndDodgeNaturally();
    void ClearActionTimers();
    void ClearDodgeTimers();
    void SetStateTag(FName Tag,bool bEnable);
    void OnActionMontageEnded(UAnimMontage* Montage,bool bInterrupted);
};
