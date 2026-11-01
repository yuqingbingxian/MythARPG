#include "MythDefenseComponent.h"
#include "MythCombatCharacter.h"
#include "MythActionComponent.h"
#include "MythAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UMythDefenseComponent::UMythDefenseComponent(){PrimaryComponentTick.bCanEverTick=false;}
void UMythDefenseComponent::EndPlay(const EEndPlayReason::Type Reason)
{
    ResetDefense();Super::EndPlay(Reason);
}
void UMythDefenseComponent::ToggleTag(const TCHAR* Name,bool bEnabled)
{
    const AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    UAbilitySystemComponent* ASC=Owner?Owner->GetAbilitySystemComponent():nullptr;
    if(!ASC)return;
    const FGameplayTag Tag=FGameplayTag::RequestGameplayTag(FName(Name),false);
    if(!Tag.IsValid())return;
    if(bEnabled)ASC->AddLooseGameplayTag(Tag);else ASC->RemoveLooseGameplayTag(Tag);
}
bool UMythDefenseComponent::BeginGuard()
{
    const AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    if(!Owner || Owner->IsDead() || Owner->IsStaggered() || !Owner->GetActionComponent() ||
       Owner->GetActionComponent()->GetActionState()!=EMythActionState::Idle)return false;
    if(bGuarding)return true;
    bGuarding=true;ToggleTag(TEXT("State.Defense.Guarding"),true);return true;
}
void UMythDefenseComponent::EndGuard()
{
    CloseParryWindow();
    if(bGuarding){bGuarding=false;ToggleTag(TEXT("State.Defense.Guarding"),false);}
}
bool UMythDefenseComponent::OpenParryWindow()
{
    if(!BeginGuard() || !GetWorld())return false;
    if(bParryActive || GetWorld()->GetTimeSeconds()-LastParryTime<ParryCooldownSeconds)return false;
    LastParryTime=GetWorld()->GetTimeSeconds();
    bParryActive=true;ToggleTag(TEXT("State.Defense.ParryWindow"),true);
    GetWorld()->GetTimerManager().SetTimer(ParryTimer,this,&UMythDefenseComponent::CloseParryWindow,ParryWindowSeconds,false);
    return true;
}
void UMythDefenseComponent::CloseParryWindow()
{
    if(GetWorld())GetWorld()->GetTimerManager().ClearTimer(ParryTimer);
    if(bParryActive){bParryActive=false;ToggleTag(TEXT("State.Defense.ParryWindow"),false);}
}
void UMythDefenseComponent::ResetDefense(){EndGuard();}
FMythDefenseResolution UMythDefenseComponent::ResolveIncoming(const AMythCombatCharacter* Attacker,float RawDamage,float RawPoise)
{
    FMythDefenseResolution Result;
    AMythCombatCharacter* Victim=Cast<AMythCombatCharacter>(GetOwner());
    if(!Victim || !Attacker || Victim->IsDead() || !bGuarding)return Result;
    const FVector ToAttacker=(Attacker->GetActorLocation()-Victim->GetActorLocation()).GetSafeNormal2D();
    if(FVector::DotProduct(Victim->GetActorForwardVector().GetSafeNormal2D(),ToAttacker)<FrontArcDotMin)return Result;
    if(bParryActive)
    {
        CloseParryWindow();Result.Outcome=EMythDefenseOutcome::Parried;
        Result.HealthMultiplier=0.f;Result.PoiseMultiplier=0.f;
        OnDefenseResolved.Broadcast(Result.Outcome);return Result;
    }
    UAbilitySystemComponent* ASC=Victim->GetAbilitySystemComponent();
    if(!ASC)return Result;
    const float Stamina=ASC->GetNumericAttribute(UMythAttributeSet::GetStaminaAttribute());
    const float Cost=FMath::Max(0.f,RawDamage)*GuardStaminaPerDamage;
    if(Stamina+KINDA_SMALL_NUMBER<Cost)
    {
        EndGuard();return Result; // Guard break: incoming attack applies normally.
    }
    if(Cost>0.f)
    {
        ASC->SetNumericAttributeBase(UMythAttributeSet::GetStaminaAttribute(),FMath::Max(0.f,Stamina-Cost));
        Victim->NotifyStaminaSpent();
    }
    Result.Outcome=EMythDefenseOutcome::Guarded;
    Result.HealthMultiplier=GuardHealthMultiplier;
    Result.PoiseMultiplier=GuardPoiseMultiplier;
    Result.StaminaSpent=Cost;
    OnDefenseResolved.Broadcast(Result.Outcome);
    return Result;
}
