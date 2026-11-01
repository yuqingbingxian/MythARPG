#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "MythComboGraph.h"
#include "TimerManager.h"
#include "MythCombatCharacter.generated.h"
class UAbilitySystemComponent;
class UMythAttributeSet;
class UMythCombatantDefinition;
class UStaticMeshComponent;
class UMythWeaponTraceComponent;
class UMythActionComponent;
struct FOnAttributeChangeData;
UENUM(BlueprintType)
enum class EMythFaction : uint8 {Player,Enemy,Neutral};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMythCharacterDeath,class AMythCombatCharacter*,Character);

UCLASS()
class MYTHGAMEPLAYRUNTIME_API AMythCombatCharacter : public ACharacter,public IAbilitySystemInterface
{
    GENERATED_BODY()
public:
    AMythCombatCharacter();
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    virtual void BeginPlay() override;
    UPROPERTY(BlueprintAssignable,Category="Combat") FMythCharacterDeath OnCharacterDeath;
    UFUNCTION(BlueprintPure,Category="Combat") bool IsDead() const{return bDead;}
    UFUNCTION(BlueprintPure,Category="Combat") bool IsStaggered() const{return bStaggered;}
    UFUNCTION(BlueprintPure,Category="Combat") float GetCurrentHealth() const;
    UFUNCTION(BlueprintPure,Category="Combat") float GetMaxHealth() const;
    UFUNCTION(BlueprintPure,Category="Combat") float GetCurrentStamina() const;
    UFUNCTION(BlueprintPure,Category="Combat") float GetCurrentPoise() const;
    UFUNCTION(BlueprintPure,Category="Combat") EMythFaction GetFaction() const{return Faction;}
    UFUNCTION(BlueprintPure,Category="Combat") UMythActionComponent* GetActionComponent() const{return ActionComponent;}
    UFUNCTION(BlueprintPure,Category="Combat") UMythWeaponTraceComponent* GetWeaponTraceComponent() const{return WeaponTrace;}
    UFUNCTION(BlueprintCallable,Category="Combat") bool RequestMeleeAbility();
    UFUNCTION(BlueprintCallable,Category="Combat") bool RequestHeavyAbility();
    UFUNCTION(BlueprintCallable,Category="Combat") bool RequestDodgeAbility();
    /** Called by weapon-trace only, once per attack window per victim. */
    void ApplyMeleeHit(AMythCombatCharacter* Target,const FHitResult& Hit,float DamageScale,float PoiseDamage);
    void NotifyStaminaSpent();
protected:
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")TObjectPtr<UAbilitySystemComponent> AbilitySystem;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")TObjectPtr<UMythAttributeSet> AttributeSet;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")TObjectPtr<UMythActionComponent> ActionComponent;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")TObjectPtr<UMythWeaponTraceComponent> WeaponTrace;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")TObjectPtr<UStaticMeshComponent> DebugMesh;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")TObjectPtr<UMythCombatantDefinition> CombatantDefinition;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")EMythFaction Faction=EMythFaction::Neutral;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")float DefaultMeleeDamage=20.f;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")bool bGrantDodge=false;
    UPROPERTY(EditDefaultsOnly,Category="Combat")float StaminaRegenDelay=0.85f;
    UPROPERTY(EditDefaultsOnly,Category="Combat")float StaggerSeconds=1.2f;
    UPROPERTY(BlueprintReadOnly,Category="Combat")bool bDead=false;
    virtual void HandleDeath();
    virtual void HandleHealthChanged(float NewHealth);
    void ReviveAt(const FTransform& TargetTransform);
    void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);
    void OnPoiseAttributeChanged(const FOnAttributeChangeData& Data);
private:
    bool bStaggered=false;
    float StaminaRegenRate=22.f;
    double LastStaminaSpendAt=-999.0;
    FTimerHandle ResourceRegenTimer,StaggerTimer;
    void RestoreResources();
    void EndStagger();
};
