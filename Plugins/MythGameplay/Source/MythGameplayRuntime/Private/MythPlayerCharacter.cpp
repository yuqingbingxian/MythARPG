#include "MythPlayerCharacter.h"
#include "MythSaveSubsystem.h"
#include "MythAttributeSet.h"
#include "MythLockOnComponent.h"
#include "MythDefenseComponent.h"
#include "MythDefenseComponent.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"

AMythPlayerCharacter::AMythPlayerCharacter()
{
    Faction=EMythFaction::Player;bGrantDodge=true;bUseControllerRotationYaw=false;
    CameraBoom=CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength=350.f;CameraBoom->bUsePawnControlRotation=true;
    FollowCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);FollowCamera->bUsePawnControlRotation=false;
    LockOnComponent=CreateDefaultSubobject<UMythLockOnComponent>(TEXT("LockOn"));
}
void AMythPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();FallbackSpawn=GetActorTransform();
    if(UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>())
    {
        FTransform Saved;float Health=100.f;
        if(Save->GetSavedPlayerState(Saved,Health))
        {
            SetActorTransform(Saved,false,nullptr,ETeleportType::TeleportPhysics);
            AbilitySystem->SetNumericAttributeBase(UMythAttributeSet::GetHealthAttribute(),
                FMath::Clamp(Health,1.f,GetMaxHealth()));
        }
    }
}
void AMythPlayerCharacter::PawnClientRestart()
{
    Super::PawnClientRestart();
    APlayerController* PC=Cast<APlayerController>(GetController());
    ULocalPlayer* LP=PC ? PC->GetLocalPlayer():nullptr;
    if(LP && DefaultInputContext)
        if(UEnhancedInputLocalPlayerSubsystem* Sub=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
        {Sub->RemoveMappingContext(DefaultInputContext);Sub->AddMappingContext(DefaultInputContext,0);}
}
void AMythPlayerCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
    Super::SetupPlayerInputComponent(Input);
    UEnhancedInputComponent* E=Cast<UEnhancedInputComponent>(Input);if(!E)return;
    if(MoveAction)E->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AMythPlayerCharacter::Move);
    if(LookAction)E->BindAction(LookAction,ETriggerEvent::Triggered,this,&AMythPlayerCharacter::Look);
    if(AttackAction)E->BindAction(AttackAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::Attack);
    if(HeavyAttackAction)E->BindAction(HeavyAttackAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::HeavyAttack);
    if(DodgeAction)E->BindAction(DodgeAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::Dodge);
    if(GuardAction){E->BindAction(GuardAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::GuardStarted);
        E->BindAction(GuardAction,ETriggerEvent::Completed,this,&AMythPlayerCharacter::GuardReleased);}
    if(ParryAction)E->BindAction(ParryAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::Parry);
    if(GuardAction){E->BindAction(GuardAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::GuardStarted);
        E->BindAction(GuardAction,ETriggerEvent::Completed,this,&AMythPlayerCharacter::GuardReleased);}
    if(ParryAction)E->BindAction(ParryAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::Parry);
    if(LockOnAction)E->BindAction(LockOnAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::ToggleLockOn);
    if(SwitchTargetLeftAction)E->BindAction(SwitchTargetLeftAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::SwitchLeft);
    if(SwitchTargetRightAction)E->BindAction(SwitchTargetRightAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::SwitchRight);
    if(SaveAction)E->BindAction(SaveAction,ETriggerEvent::Started,this,&AMythPlayerCharacter::SaveInput);
}
void AMythPlayerCharacter::Move(const FInputActionValue& Value)
{
    if(bDead || !Controller)return;
    const FVector2D Axis=Value.Get<FVector2D>();
    const FRotator Yaw(0.f,Controller->GetControlRotation().Yaw,0.f);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X),Axis.Y);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y),Axis.X);
}
void AMythPlayerCharacter::Look(const FInputActionValue& Value)
{
    if(bDead || (LockOnComponent && LockOnComponent->GetTarget()))return;
    const FVector2D Axis=Value.Get<FVector2D>();
    AddControllerYawInput(Axis.X);AddControllerPitchInput(Axis.Y);
}
void AMythPlayerCharacter::Attack(const FInputActionValue&){RequestMeleeAbility();}
void AMythPlayerCharacter::HeavyAttack(const FInputActionValue&){RequestHeavyAbility();}
void AMythPlayerCharacter::Dodge(const FInputActionValue&){RequestDodgeAbility();}
void AMythPlayerCharacter::GuardStarted(const FInputActionValue&){if(DefenseComponent)DefenseComponent->BeginGuard();}
void AMythPlayerCharacter::GuardReleased(const FInputActionValue&){if(DefenseComponent)DefenseComponent->EndGuard();}
void AMythPlayerCharacter::Parry(const FInputActionValue&){if(DefenseComponent)DefenseComponent->OpenParryWindow();}
void AMythPlayerCharacter::GuardStarted(const FInputActionValue&){if(DefenseComponent)DefenseComponent->BeginGuard();}
void AMythPlayerCharacter::GuardReleased(const FInputActionValue&){if(DefenseComponent)DefenseComponent->EndGuard();}
void AMythPlayerCharacter::Parry(const FInputActionValue&){if(DefenseComponent)DefenseComponent->OpenParryWindow();}
void AMythPlayerCharacter::ToggleLockOn(const FInputActionValue&){if(LockOnComponent)LockOnComponent->ToggleLockOn();}
void AMythPlayerCharacter::SwitchLeft(const FInputActionValue&){if(LockOnComponent)LockOnComponent->CycleTarget(false);}
void AMythPlayerCharacter::SwitchRight(const FInputActionValue&){if(LockOnComponent)LockOnComponent->CycleTarget(true);}
void AMythPlayerCharacter::SaveInput(const FInputActionValue&){SaveAtCurrentLocation();}
void AMythPlayerCharacter::SaveAtCurrentLocation()
{
    if(bDead)return;
    if(UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>())
        Save->SavePlayerState(GetActorTransform(),GetCurrentHealth());
}
void AMythPlayerCharacter::HandleDeath()
{
    if(LockOnComponent)LockOnComponent->ClearLockOn();
    Super::HandleDeath();
    GetWorldTimerManager().SetTimer(RespawnTimer,this,&AMythPlayerCharacter::Respawn,2.0f,false);
}
void AMythPlayerCharacter::Respawn()
{
    FTransform Where=FallbackSpawn;
    if(UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>())
    {
        float SavedHealth=0.f;FTransform Saved;
        if(Save->GetSavedPlayerState(Saved,SavedHealth))Where=Saved;
    }
    ReviveAt(Where);
}
