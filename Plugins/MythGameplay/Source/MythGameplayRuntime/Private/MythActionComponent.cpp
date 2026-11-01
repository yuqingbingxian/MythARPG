#include "MythActionComponent.h"
#include "MythCombatCharacter.h"
#include "MythAttributeSet.h"
#include "MythMeleeAbility.h"
#include "MythDodgeAbility.h"
#include "MythWeaponTraceComponent.h"
#include "MythDefenseComponent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "Engine/World.h"
#include "TimerManager.h"

UMythActionComponent::UMythActionComponent() {PrimaryComponentTick.bCanEverTick=false;}
void UMythActionComponent::BeginPlay()
{
    Super::BeginPlay();
    if (ComboGraph)
    {
        FString ValidationError;
        ensureMsgf(ComboGraph->ValidateGraph(ValidationError),TEXT("Invalid Myth combo graph: %s"),*ValidationError);
    }
    // Always playable with built-in debug cube: 3 light hits + one heavy attack.
    FMythComboNode N; N.NodeId=TEXT("Light1"); N.EntryInput=EMythAttackInput::Light;
    N.Duration=.70f; N.HitOpen=.14f;N.HitClose=.33f;N.ComboOpen=.30f;N.ComboClose=.58f;
    N.DodgeCancelOpen=.35f;N.DodgeCancelClose=.65f;
    FallbackNodes.Add(N);
    N.NodeId=TEXT("Light2");N.DamageScale=1.15f;N.PoiseDamage=28.f;FallbackNodes.Add(N);
    N.NodeId=TEXT("Light3");N.DamageScale=1.5f;N.StaminaCost=17.f;N.PoiseDamage=45.f;FallbackNodes.Add(N);
    N.NodeId=TEXT("Heavy1");N.EntryInput=EMythAttackInput::Heavy;N.Duration=1.0f;
    N.HitOpen=.4f;N.HitClose=.62f;N.ComboOpen=.6f;N.ComboClose=.85f;
    N.DodgeCancelOpen=.72f;N.DodgeCancelClose=.92f;
    N.DamageScale=2.f;N.PoiseDamage=65.f;N.StaminaCost=25.f;
    FallbackNodes.Add(N);
}
const FMythComboNode* UMythActionComponent::FindEntry(EMythAttackInput Input) const
{
    if (ComboGraph) return ComboGraph->FindEntry(Input);
    return FallbackNodes.FindByPredicate([Input](const FMythComboNode& N){return N.EntryInput==Input;});
}
const FMythComboNode* UMythActionComponent::FindNext(FName Id,EMythAttackInput Input) const
{
    if (ComboGraph) return ComboGraph->FindNext(Id,Input);
    if (Input == EMythAttackInput::Light)
    {
        if (Id==TEXT("Light1")) return FallbackNodes.FindByPredicate([](const FMythComboNode& N){return N.NodeId==TEXT("Light2");});
        if (Id==TEXT("Light2")) return FallbackNodes.FindByPredicate([](const FMythComboNode& N){return N.NodeId==TEXT("Light3");});
    }
    if (Input==EMythAttackInput::Heavy && Id==TEXT("Light2"))
        return FallbackNodes.FindByPredicate([](const FMythComboNode& N){return N.NodeId==TEXT("Heavy1");});
    return nullptr;
}
void UMythActionComponent::SetStateTag(FName Tag,bool bEnable)
{
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    UAbilitySystemComponent* ASC=Owner ? Owner->GetAbilitySystemComponent() : nullptr;
    if (!ASC) return;
    const FGameplayTag GameplayTag=FGameplayTag::RequestGameplayTag(Tag);
    if (bEnable) ASC->AddLooseGameplayTag(GameplayTag);
    else ASC->RemoveLooseGameplayTag(GameplayTag);
}
bool UMythActionComponent::SpendStamina(float Amount)
{
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    if (!Owner || Owner->IsDead()) return false;
    if (Amount <= 0.f) return true;
    UAbilitySystemComponent* ASC=Owner->GetAbilitySystemComponent();
    if (!ASC) return false;
    const float Current=ASC->GetNumericAttribute(UMythAttributeSet::GetStaminaAttribute());
    if (Current+KINDA_SMALL_NUMBER < Amount) return false;
    // Prototype direct base modification; can be replaced by a resource GameplayEffect policy.
    ASC->SetNumericAttributeBase(UMythAttributeSet::GetStaminaAttribute(),FMath::Max(0.f,Current-Amount));
    Owner->NotifyStaminaSpent();
    return true;
}
bool UMythActionComponent::RequestAttack(EMythAttackInput Input)
{
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    if (!Owner || Owner->IsDead() || Owner->IsStaggered() ||
        (Owner->GetDefenseComponent() && Owner->GetDefenseComponent()->IsGuarding()))return false;
    if (State==EMythActionState::Attacking)
    {
        if (!FindNext(CurrentNode.NodeId,Input)) return false;
        if (bComboWindow) return TryAdvance(Input);
        bBuffered=true;BufferedInput=Input;BufferedAt=GetWorld()->GetTimeSeconds();
        return true;
    }
    if (State!=EMythActionState::Idle) return false;
    RequestedEntry=Input;
    return Owner->GetAbilitySystemComponent()->TryActivateAbilityByClass(UMythMeleeAbility::StaticClass());
}
bool UMythActionComponent::RequestDodge()
{
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    if (!Owner || Owner->IsDead() || Owner->IsStaggered() || State==EMythActionState::Dodging) return false;
    if (State==EMythActionState::Attacking)
    {
        if (!bDodgeCancelWindow) return false;
        // Do not destroy the current combo if the dodge cannot afford its cost.
        UAbilitySystemComponent* ASC=Owner->GetAbilitySystemComponent();
        if (!ASC || ASC->GetNumericAttribute(UMythAttributeSet::GetStaminaAttribute())+KINDA_SMALL_NUMBER < DodgeStaminaCost) return false;
        if (UMythMeleeAbility* Ability=AttackAbility.Get()) Ability->CancelFromAction();
        if (State!=EMythActionState::Idle) return false;
    }
    return Owner->GetAbilitySystemComponent()->TryActivateAbilityByClass(UMythDodgeAbility::StaticClass());
}
bool UMythActionComponent::BeginAttack(UMythMeleeAbility* Ability)
{
    if (State!=EMythActionState::Idle || !Ability) return false;
    const FMythComboNode* Entry=FindEntry(RequestedEntry);
    if (!Entry) return false;
    AttackAbility=Ability;
    State=EMythActionState::Attacking;
    SetStateTag(TEXT("State.Combat.Attacking"),true);
    if (!StartNode(*Entry)) {AbortAttack(Ability);return false;}
    return true;
}
void UMythActionComponent::ClearActionTimers()
{
    if (!GetWorld()) return;
    FTimerManager& T=GetWorld()->GetTimerManager();
    T.ClearTimer(HitOpenTimer);T.ClearTimer(HitCloseTimer);
    T.ClearTimer(ComboOpenTimer);T.ClearTimer(ComboCloseTimer);
    T.ClearTimer(CancelOpenTimer);T.ClearTimer(CancelCloseTimer);T.ClearTimer(ActionEndTimer);
}
bool UMythActionComponent::StartNode(const FMythComboNode& Node)
{
    if (!SpendStamina(Node.StaminaCost)) return false;
    ClearActionTimers();
    CloseHitWindow();
    bComboWindow=false;bDodgeCancelWindow=false;bBuffered=false;
    CurrentNode=Node;
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    UAnimInstance* Anim=Owner && Owner->GetMesh() ? Owner->GetMesh()->GetAnimInstance() : nullptr;
    UAnimMontage* PreviousMontage=ActiveMontage;
    ActiveMontage=nullptr;
    if (Anim && PreviousMontage) Anim->Montage_Stop(.08f,PreviousMontage);
    ActiveMontage=nullptr;
    bool bPlayed=false;
    if (Anim && Node.Montage)
    {
        bPlayed=Anim->Montage_Play(Node.Montage)>0.f;
        if (bPlayed && !Node.MontageSection.IsNone()) Anim->Montage_JumpToSection(Node.MontageSection,Node.Montage);
        if (bPlayed)
        {
            ActiveMontage=Node.Montage;
            FOnMontageEnded End;End.BindUObject(this,&UMythActionComponent::OnActionMontageEnded);
            Anim->Montage_SetEndDelegate(End,Node.Montage);
        }
    }
    if (!GetWorld()) return false;
    FTimerManager& Timer=GetWorld()->GetTimerManager();
    if (!(Node.bUseAnimationNotifies && bPlayed))
    {
        Timer.SetTimer(HitOpenTimer,this,&UMythActionComponent::OpenHitWindow,FMath::Max(.001f,Node.HitOpen),false);
        Timer.SetTimer(HitCloseTimer,this,&UMythActionComponent::CloseHitWindow,FMath::Max(.002f,Node.HitClose),false);
        Timer.SetTimer(ComboOpenTimer,this,&UMythActionComponent::OpenComboWindow,FMath::Max(.001f,Node.ComboOpen),false);
        Timer.SetTimer(ComboCloseTimer,this,&UMythActionComponent::CloseComboWindow,FMath::Max(.002f,Node.ComboClose),false);
    }
    if (!(Node.bUseAnimationNotifies && bPlayed))
    {
        Timer.SetTimer(CancelOpenTimer,this,&UMythActionComponent::OpenDodgeCancelWindow,FMath::Max(.001f,Node.DodgeCancelOpen),false);
        Timer.SetTimer(CancelCloseTimer,this,&UMythActionComponent::CloseDodgeCancelWindow,FMath::Max(.002f,Node.DodgeCancelClose),false);
    }
    Timer.SetTimer(ActionEndTimer,this,&UMythActionComponent::EndAttackNaturally,Node.Duration,false);
    return true;
}
bool UMythActionComponent::TryAdvance(EMythAttackInput Input)
{
    if (State!=EMythActionState::Attacking || !bComboWindow) return false;
    const FMythComboNode* Next=FindNext(CurrentNode.NodeId,Input);
    return Next ? StartNode(*Next) : false;
}
void UMythActionComponent::ConsumeBuffered()
{
    if (!bBuffered || !GetWorld()) return;
    const EMythAttackInput Input=BufferedInput;
    const bool bFresh=GetWorld()->GetTimeSeconds()-BufferedAt<=InputBufferSeconds;
    bBuffered=false;
    if (bFresh) TryAdvance(Input);
}
void UMythActionComponent::OpenHitWindow()
{
    if (State!=EMythActionState::Attacking || bHitWindow) return;
    bHitWindow=true;
    if (AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner()))
        if (UMythWeaponTraceComponent* Trace=Owner->GetWeaponTraceComponent())
            Trace->BeginTrace(CurrentNode.DamageScale,CurrentNode.PoiseDamage);
}
void UMythActionComponent::CloseHitWindow()
{
    if (!bHitWindow) return;
    bHitWindow=false;
    if (AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner()))
        if (UMythWeaponTraceComponent* Trace=Owner->GetWeaponTraceComponent()) Trace->EndTrace();
}
void UMythActionComponent::OpenComboWindow()
{
    if (State==EMythActionState::Attacking) {bComboWindow=true;ConsumeBuffered();}
}
void UMythActionComponent::CloseComboWindow(){bComboWindow=false;}
void UMythActionComponent::OpenDodgeCancelWindow(){if(State==EMythActionState::Attacking)bDodgeCancelWindow=true;}
void UMythActionComponent::CloseDodgeCancelWindow(){bDodgeCancelWindow=false;}
void UMythActionComponent::EndAttackNaturally()
{
    if (UMythMeleeAbility* Ability=AttackAbility.Get()) Ability->CompleteFromAction();
    else ForceReset();
}
void UMythActionComponent::AbortAttack(UMythMeleeAbility* Ability)
{
    if (State!=EMythActionState::Attacking || AttackAbility.Get()!=Ability) return;
    ClearActionTimers();CloseHitWindow();
    UAnimMontage* MontageToStop=ActiveMontage;ActiveMontage=nullptr;
    if (AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner()))
        if (UAnimInstance* Anim=Owner->GetMesh() ? Owner->GetMesh()->GetAnimInstance():nullptr)
            if (MontageToStop) Anim->Montage_Stop(.1f,MontageToStop);
    ActiveMontage=nullptr;
    bBuffered=false;bComboWindow=false;bDodgeCancelWindow=false;
    CurrentNode=FMythComboNode();
    AttackAbility.Reset();State=EMythActionState::Idle;
    SetStateTag(TEXT("State.Combat.Attacking"),false);
}
bool UMythActionComponent::BeginDodge(UMythDodgeAbility* Ability)
{
    if (State!=EMythActionState::Idle || !Ability || !SpendStamina(DodgeStaminaCost)) return false;
    if(AMythCombatCharacter* Combatant=Cast<AMythCombatCharacter>(GetOwner()))
        if(UMythDefenseComponent* Defense=Combatant->GetDefenseComponent())Defense->ResetDefense();
    State=EMythActionState::Dodging;DodgeAbility=Ability;
    SetStateTag(TEXT("State.Movement.Dodging"),true);
    ACharacter* Owner=Cast<ACharacter>(GetOwner());
    if (Owner)
    {
        FVector Direction=Owner->GetLastMovementInputVector().GetSafeNormal2D();
        if (Direction.IsNearlyZero()) Direction=Owner->GetActorForwardVector();
        Owner->LaunchCharacter(Direction*DodgeImpulse,true,false);
    }
    if (!GetWorld()) return false;
    FTimerManager& Timer=GetWorld()->GetTimerManager();
    UAnimInstance* Anim=Owner && Owner->GetMesh() ? Owner->GetMesh()->GetAnimInstance():nullptr;
    const bool bPlayed=Anim && DodgeMontage && Anim->Montage_Play(DodgeMontage)>0.f;
    ActiveMontage=bPlayed ? DodgeMontage : nullptr;
    if(bPlayed)
    {
        FOnMontageEnded End;End.BindUObject(this,&UMythActionComponent::OnActionMontageEnded);
        Anim->Montage_SetEndDelegate(End,DodgeMontage);
    }
    if (!(bDodgeUseAnimationNotifies && bPlayed))
    {
        Timer.SetTimer(IFrameOpenTimer,this,&UMythActionComponent::OpenDodgeIFrames,FMath::Max(.001f,DodgeIFrameOpen),false);
        Timer.SetTimer(IFrameCloseTimer,this,&UMythActionComponent::CloseDodgeIFrames,FMath::Max(.002f,DodgeIFrameClose),false);
    }
    Timer.SetTimer(ActionEndTimer,this,&UMythActionComponent::EndDodgeNaturally,DodgeDuration,false);
    return true;
}
void UMythActionComponent::OpenDodgeIFrames()
{
    if (State==EMythActionState::Dodging && !bIFrames)
    {bIFrames=true;SetStateTag(TEXT("State.Defense.Invulnerable"),true);}
}
void UMythActionComponent::CloseDodgeIFrames()
{
    if (bIFrames) {bIFrames=false;SetStateTag(TEXT("State.Defense.Invulnerable"),false);}
}
void UMythActionComponent::ClearDodgeTimers()
{
    if (!GetWorld()) return;
    FTimerManager& T=GetWorld()->GetTimerManager();
    T.ClearTimer(IFrameOpenTimer);T.ClearTimer(IFrameCloseTimer);T.ClearTimer(ActionEndTimer);
}
void UMythActionComponent::EndDodgeNaturally()
{
    if (UMythDodgeAbility* Ability=DodgeAbility.Get()) Ability->CompleteFromAction();
    else ForceReset();
}
void UMythActionComponent::AbortDodge(UMythDodgeAbility* Ability)
{
    if (State!=EMythActionState::Dodging || DodgeAbility.Get()!=Ability) return;
    ClearDodgeTimers();CloseDodgeIFrames();
    UAnimMontage* MontageToStop=ActiveMontage;ActiveMontage=nullptr;
    if (AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner()))
        if (UAnimInstance* Anim=Owner->GetMesh() ? Owner->GetMesh()->GetAnimInstance():nullptr)
            if (MontageToStop) Anim->Montage_Stop(.1f,MontageToStop);
    ActiveMontage=nullptr;
    DodgeAbility.Reset();State=EMythActionState::Idle;
    SetStateTag(TEXT("State.Movement.Dodging"),false);
}
void UMythActionComponent::ForceReset()
{
    if (UMythMeleeAbility* A=AttackAbility.Get()) {AbortAttack(A);return;}
    if (UMythDodgeAbility* A=DodgeAbility.Get()) {AbortDodge(A);return;}
    const EMythActionState Previous=State;
    ClearActionTimers();ClearDodgeTimers();CloseHitWindow();CloseDodgeIFrames();
    bBuffered=false;bComboWindow=false;bDodgeCancelWindow=false;
    AttackAbility.Reset();DodgeAbility.Reset();ActiveMontage=nullptr;
    CurrentNode=FMythComboNode();State=EMythActionState::Idle;
    if(Previous==EMythActionState::Attacking)SetStateTag(TEXT("State.Combat.Attacking"),false);
    if(Previous==EMythActionState::Dodging)SetStateTag(TEXT("State.Movement.Dodging"),false);
}

void UMythActionComponent::OnActionMontageEnded(UAnimMontage* Montage,bool bInterrupted)
{
    if(!Montage || Montage!=ActiveMontage)return; // Ignore an old combo section finishing late.
    ActiveMontage=nullptr;
    if(State==EMythActionState::Attacking)
    {
        if(bInterrupted)
        {
            if(UMythMeleeAbility* Ability=AttackAbility.Get())Ability->CancelFromAction();
            else ForceReset();
        }
        else EndAttackNaturally();
    }
    else if(State==EMythActionState::Dodging)
    {
        if(bInterrupted)
        {
            if(UMythDodgeAbility* Ability=DodgeAbility.Get())Ability->CancelFromAction();
            else ForceReset();
        }
        else EndDodgeNaturally();
    }
}
