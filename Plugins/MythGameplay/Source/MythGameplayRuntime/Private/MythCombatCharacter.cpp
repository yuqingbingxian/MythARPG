#include "MythCombatCharacter.h"
#include "MythAttributeSet.h"
#include "MythCombatantDefinition.h"
#include "MythDamageEffect.h"
#include "MythCombatMath.h"
#include "MythMeleeAbility.h"
#include "MythDodgeAbility.h"
#include "MythActionComponent.h"
#include "MythWeaponTraceComponent.h"
#include "MythDefenseComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

AMythCombatCharacter::AMythCombatCharacter()
{
    PrimaryActorTick.bCanEverTick=false;
    AbilitySystem=CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
    AttributeSet=CreateDefaultSubobject<UMythAttributeSet>(TEXT("AttributeSet"));
    ActionComponent=CreateDefaultSubobject<UMythActionComponent>(TEXT("ActionComponent"));
    WeaponTrace=CreateDefaultSubobject<UMythWeaponTraceComponent>(TEXT("WeaponTrace"));
    DefenseComponent=CreateDefaultSubobject<UMythDefenseComponent>(TEXT("Defense"));
    DebugMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugVisual"));
    DebugMesh->SetupAttachment(GetRootComponent());
    DebugMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DebugMesh->SetRelativeScale3D(FVector(.5f,.5f,1.6f));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if(Cube.Succeeded())DebugMesh->SetStaticMesh(Cube.Object);
    GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);
    GetCapsuleComponent()->SetCapsuleRadius(42.f);
    GetCharacterMovement()->bOrientRotationToMovement=true;
}
UAbilitySystemComponent* AMythCombatCharacter::GetAbilitySystemComponent() const{return AbilitySystem;}
void AMythCombatCharacter::BeginPlay()
{
    Super::BeginPlay();
    AbilitySystem->InitAbilityActorInfo(this,this);
    if(CombatantDefinition)
    {
        AttributeSet->InitMaxHealth(FMath::Max(1.f,CombatantDefinition->MaxHealth));
        AttributeSet->InitHealth(AttributeSet->GetMaxHealth());
        AttributeSet->InitMaxStamina(FMath::Max(1.f,CombatantDefinition->MaxStamina));
        AttributeSet->InitStamina(AttributeSet->GetMaxStamina());
        AttributeSet->InitMaxPoise(FMath::Max(1.f,CombatantDefinition->MaxPoise));
        AttributeSet->InitPoise(AttributeSet->GetMaxPoise());
        AttributeSet->InitDefense(FMath::Max(0.f,CombatantDefinition->Defense));
        DefaultMeleeDamage=CombatantDefinition->MeleeDamage;
        StaminaRegenRate=CombatantDefinition->StaminaRegenPerSecond;
    }
    AbilitySystem->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetHealthAttribute())
        .AddUObject(this,&AMythCombatCharacter::OnHealthAttributeChanged);
    AbilitySystem->GetGameplayAttributeValueChangeDelegate(UMythAttributeSet::GetPoiseAttribute())
        .AddUObject(this,&AMythCombatCharacter::OnPoiseAttributeChanged);
    if(HasAuthority())
    {
        AbilitySystem->GiveAbility(FGameplayAbilitySpec(UMythMeleeAbility::StaticClass(),1));
        if(bGrantDodge)AbilitySystem->GiveAbility(FGameplayAbilitySpec(UMythDodgeAbility::StaticClass(),1));
    }
    GetWorldTimerManager().SetTimer(ResourceRegenTimer,this,&AMythCombatCharacter::RestoreResources,.10f,true);
}
float AMythCombatCharacter::GetCurrentHealth()const{return AttributeSet->GetHealth();}
float AMythCombatCharacter::GetMaxHealth()const{return AttributeSet->GetMaxHealth();}
float AMythCombatCharacter::GetCurrentStamina()const{return AttributeSet->GetStamina();}
float AMythCombatCharacter::GetCurrentPoise()const{return AttributeSet->GetPoise();}
bool AMythCombatCharacter::RequestMeleeAbility(){return ActionComponent && ActionComponent->RequestAttack(EMythAttackInput::Light);}
bool AMythCombatCharacter::RequestHeavyAbility(){return ActionComponent && ActionComponent->RequestAttack(EMythAttackInput::Heavy);}
bool AMythCombatCharacter::RequestDodgeAbility(){return ActionComponent && ActionComponent->RequestDodge();}
void AMythCombatCharacter::NotifyStaminaSpent()
{
    if(GetWorld())LastStaminaSpendAt=GetWorld()->GetTimeSeconds();
}
void AMythCombatCharacter::RestoreResources()
{
    if(bDead || !GetWorld())return;
    if(GetWorld()->GetTimeSeconds()-LastStaminaSpendAt>=StaminaRegenDelay)
    {
        const float Next=FMath::Min(AttributeSet->GetMaxStamina(),AttributeSet->GetStamina()+StaminaRegenRate*.1f);
        if(Next>AttributeSet->GetStamina())AbilitySystem->SetNumericAttributeBase(UMythAttributeSet::GetStaminaAttribute(),Next);
    }
    if(!bStaggered && AttributeSet->GetPoise()<AttributeSet->GetMaxPoise())
    {
        const float Next=FMath::Min(AttributeSet->GetMaxPoise(),AttributeSet->GetPoise()+8.f*.1f);
        AbilitySystem->SetNumericAttributeBase(UMythAttributeSet::GetPoiseAttribute(),Next);
    }
}
void AMythCombatCharacter::ApplyMeleeHit(AMythCombatCharacter* Target,const FHitResult& Hit,float Scale,float PoiseAmount)
{
    if(!Target || Target==this || bDead || Target->IsDead() || Target->Faction==Faction ||
       Faction==EMythFaction::Neutral || Target->Faction==EMythFaction::Neutral)return;
    if(Target->AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("State.Defense.Invulnerable")))))return;
    const float RawDamage=FMath::Max(0.f,DefaultMeleeDamage*Scale);
    const FMythDefenseResolution Defense=Target->DefenseComponent
        ? Target->DefenseComponent->ResolveIncoming(this,RawDamage,PoiseAmount)
        : FMythDefenseResolution();
    if(Defense.Outcome==EMythDefenseOutcome::Parried)
    {
        AbilitySystem->CancelAllAbilities();ActionComponent->ForceReset();
        return;
    }
    const float Damage=FMythCombatMath::MitigatedDamage(RawDamage*Defense.HealthMultiplier,
        Target->AttributeSet->GetDefense());
    PoiseAmount*=Defense.PoiseMultiplier;
    if(Damage<=0.f && PoiseAmount<=0.f)return;
    FGameplayEffectContextHandle Context=AbilitySystem->MakeEffectContext();
    Context.AddSourceObject(this);
    Context.AddHitResult(Hit);
    FGameplayEffectSpecHandle Spec=AbilitySystem->MakeOutgoingSpec(UMythDamageEffect::StaticClass(),1.f,Context);
    if(Spec.IsValid() && Spec.Data.IsValid())
    {
        Spec.Data->SetSetByCallerMagnitude(FName(TEXT("Damage")),-FMath::Max(0.f,Damage));
        Spec.Data->SetSetByCallerMagnitude(FName(TEXT("PoiseDamage")),-FMath::Max(0.f,PoiseAmount));
        Target->AbilitySystem->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    }
}
void AMythCombatCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
    HandleHealthChanged(Data.NewValue);
    if(!bDead && Data.NewValue<=0.f)HandleDeath();
}
void AMythCombatCharacter::OnPoiseAttributeChanged(const FOnAttributeChangeData& Data)
{
    if(bDead || bStaggered || Data.NewValue>0.f)return;
    bStaggered=true;
    if(DefenseComponent)DefenseComponent->ResetDefense();
    AbilitySystem->CancelAllAbilities();
    ActionComponent->ForceReset();
    AbilitySystem->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("State.Combat.Staggered"))));
    GetWorldTimerManager().SetTimer(StaggerTimer,this,&AMythCombatCharacter::EndStagger,StaggerSeconds,false);
}
void AMythCombatCharacter::EndStagger()
{
    if(!bStaggered)return;
    bStaggered=false;
    AbilitySystem->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("State.Combat.Staggered"))));
    AbilitySystem->SetNumericAttributeBase(UMythAttributeSet::GetPoiseAttribute(),AttributeSet->GetMaxPoise());
}
void AMythCombatCharacter::HandleHealthChanged(float NewHealth){}
void AMythCombatCharacter::HandleDeath()
{
    if(bDead)return;
    bDead=true;
    AbilitySystem->CancelAllAbilities();
    ActionComponent->ForceReset();
    if(DefenseComponent)DefenseComponent->ResetDefense();
    GetWorldTimerManager().ClearTimer(StaggerTimer);
    if(bStaggered)EndStagger();
    GetCharacterMovement()->DisableMovement();
    AbilitySystem->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("State.Character.Dead"))));
    OnCharacterDeath.Broadcast(this);
}
void AMythCombatCharacter::ReviveAt(const FTransform& Where)
{
    AbilitySystem->CancelAllAbilities();ActionComponent->ForceReset();
    bDead=false;
    if(DefenseComponent)DefenseComponent->ResetDefense();
    SetActorTransform(Where,false,nullptr,ETeleportType::TeleportPhysics);
    AbilitySystem->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("State.Character.Dead"))));
    AbilitySystem->SetNumericAttributeBase(UMythAttributeSet::GetHealthAttribute(),GetMaxHealth());
    AbilitySystem->SetNumericAttributeBase(UMythAttributeSet::GetStaminaAttribute(),AttributeSet->GetMaxStamina());
    EndStagger();
    AbilitySystem->SetNumericAttributeBase(UMythAttributeSet::GetPoiseAttribute(),AttributeSet->GetMaxPoise());
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
