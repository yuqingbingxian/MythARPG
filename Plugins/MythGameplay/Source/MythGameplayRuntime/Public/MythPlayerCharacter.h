#pragma once
#include "CoreMinimal.h"
#include "MythCombatCharacter.h"
#include "InputActionValue.h"
#include "TimerManager.h"
#include "MythPlayerCharacter.generated.h"
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UMythLockOnComponent;

UCLASS()
class MYTHGAMEPLAYRUNTIME_API AMythPlayerCharacter : public AMythCombatCharacter
{
    GENERATED_BODY()
public:
    AMythPlayerCharacter();
    virtual void BeginPlay()override;
    virtual void PawnClientRestart()override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)override;
    UFUNCTION(BlueprintCallable,Category="Save") void SaveAtCurrentLocation();
protected:
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)TObjectPtr<UCameraComponent> FollowCamera;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)TObjectPtr<UMythLockOnComponent> LockOnComponent;
    UPROPERTY(EditDefaultsOnly,Category="Input")TObjectPtr<UInputMappingContext> DefaultInputContext;
    UPROPERTY(EditDefaultsOnly,Category="Input")TObjectPtr<UInputAction> MoveAction;
    UPROPERTY(EditDefaultsOnly,Category="Input")TObjectPtr<UInputAction> LookAction;
    UPROPERTY(EditDefaultsOnly,Category="Input")TObjectPtr<UInputAction> AttackAction;
    UPROPERTY(EditDefaultsOnly,Category="Input")TObjectPtr<UInputAction> HeavyAttackAction;
    UPROPERTY(EditDefaultsOnly,Category="Input")TObjectPtr<UInputAction> DodgeAction;
    UPROPERTY(EditDefaultsOnly,Category="Input")TObjectPtr<UInputAction> LockOnAction;
    UPROPERTY(EditDefaultsOnly,Category="Input")TObjectPtr<UInputAction> SwitchTargetLeftAction;
    UPROPERTY(EditDefaultsOnly,Category="Input")TObjectPtr<UInputAction> SwitchTargetRightAction;
    UPROPERTY(EditDefaultsOnly,Category="Input")TObjectPtr<UInputAction> SaveAction;
    virtual void HandleDeath()override;
private:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Attack(const FInputActionValue& Value);
    void HeavyAttack(const FInputActionValue& Value);
    void Dodge(const FInputActionValue& Value);
    void ToggleLockOn(const FInputActionValue& Value);
    void SwitchLeft(const FInputActionValue& Value);
    void SwitchRight(const FInputActionValue& Value);
    void SaveInput(const FInputActionValue& Value);
    void Respawn();
    FTransform FallbackSpawn;
    FTimerHandle RespawnTimer;
};
