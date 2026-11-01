#include "MythExecutionComponent.h"
#include "MythCombatCharacter.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

UMythExecutionComponent::UMythExecutionComponent(){PrimaryComponentTick.bCanEverTick=false;}
bool UMythExecutionComponent::BeginExecution(AMythCombatCharacter* Victim,UAnimMontage* AttackerMontage,
    UAnimMontage* VictimMontage,FName WarpTargetName,float MaxDistance)
{
    AMythCombatCharacter* Attacker=Cast<AMythCombatCharacter>(GetOwner());
    if(!Attacker || !Victim || Victim==Attacker || Attacker->IsDead() || Victim->IsDead() ||
       bBusy || !AttackerMontage || !VictimMontage || WarpTargetName.IsNone() || MaxDistance<=0.f ||
       FVector::Dist(Attacker->GetActorLocation(),Victim->GetActorLocation())>MaxDistance)return false;
    UMythExecutionComponent* Other=Victim->GetExecutionComponent();
    UAnimInstance* A=Attacker->GetMesh()?Attacker->GetMesh()->GetAnimInstance():nullptr;
    UAnimInstance* B=Victim->GetMesh()?Victim->GetMesh()->GetAnimInstance():nullptr;
    UMotionWarpingComponent* Warp=Attacker->GetMotionWarpingComponent();
    if(!Other || Other->bBusy || !A || !B || !Warp)return false;

    Attacker->GetAbilitySystemComponent()->CancelAllAbilities();
    Victim->GetAbilitySystemComponent()->CancelAllAbilities();
    bBusy=true;Other->bBusy=true;Partner=Other;Other->Partner=this;
    VictimActor=Victim;AttackerMontageActive=AttackerMontage;VictimMontageActive=VictimMontage;
    bAppliedImpact=false;ActiveWarpTarget=WarpTargetName;
    Warp->AddOrUpdateWarpTargetFromTransform(WarpTargetName,Victim->GetActorTransform());
    const bool bA=A->Montage_Play(AttackerMontage)>0.f;
    const bool bB=bA && B->Montage_Play(VictimMontage)>0.f;
    if(!bA || !bB){ReleasePair(true);return false;}
    FOnMontageEnded EndA;EndA.BindUObject(this,&UMythExecutionComponent::OnAttackerMontageEnded);
    FOnMontageEnded EndB;EndB.BindUObject(this,&UMythExecutionComponent::OnVictimMontageEnded);
    A->Montage_SetEndDelegate(EndA,AttackerMontage);
    B->Montage_SetEndDelegate(EndB,VictimMontage);
    return true;
}
bool UMythExecutionComponent::CommitImpact(float DamageScale,float PoiseDamage)
{
    AMythCombatCharacter* Attacker=Cast<AMythCombatCharacter>(GetOwner());
    AMythCombatCharacter* Victim=VictimActor.Get();
    if(!bBusy || bAppliedImpact || !Attacker || !Victim || Attacker->IsDead() || Victim->IsDead())return false;
    bAppliedImpact=true;
    Attacker->ApplyMeleeHit(Victim,FHitResult(),FMath::Max(0.f,DamageScale),FMath::Max(0.f,PoiseDamage));
    return true;
}
void UMythExecutionComponent::OnAttackerMontageEnded(UAnimMontage* Montage,bool bInterrupted)
{if(Montage==AttackerMontageActive)ReleasePair(true);}
void UMythExecutionComponent::OnVictimMontageEnded(UAnimMontage* Montage,bool bInterrupted)
{if(Montage==VictimMontageActive)ReleasePair(true);}
void UMythExecutionComponent::CancelExecution(){ReleasePair(true);}
void UMythExecutionComponent::ReleasePair(bool bStopMontages)
{
    if(bCleaning || !bBusy)return;
    bCleaning=true;
    UMythExecutionComponent* Other=Partner.Get();
    // Drop state before stopping montages: stopping can invoke EndDelegate synchronously.
    bBusy=false;Partner.Reset();VictimActor.Reset();
    UAnimMontage* AOld=AttackerMontageActive;UAnimMontage* BOld=VictimMontageActive;
    AttackerMontageActive=nullptr;VictimMontageActive=nullptr;
    if(Other){Other->bBusy=false;Other->Partner.Reset();}
    if(AMythCombatCharacter* Attacker=Cast<AMythCombatCharacter>(GetOwner()))
    {
        if(UMotionWarpingComponent* Warp=Attacker->GetMotionWarpingComponent())
            if(!ActiveWarpTarget.IsNone())Warp->RemoveWarpTarget(ActiveWarpTarget);
        if(bStopMontages && Attacker->GetMesh())
            if(UAnimInstance* Anim=Attacker->GetMesh()->GetAnimInstance())
                if(AOld)Anim->Montage_Stop(.1f,AOld);
    }
    if(bStopMontages && Other)
        if(AMythCombatCharacter* Target=Cast<AMythCombatCharacter>(Other->GetOwner()))
            if(Target->GetMesh())
                if(UAnimInstance* Anim=Target->GetMesh()->GetAnimInstance())
                    if(BOld)Anim->Montage_Stop(.1f,BOld);
    ActiveWarpTarget=NAME_None;bCleaning=false;
}
void UMythExecutionComponent::EndPlay(const EEndPlayReason::Type Reason)
{CancelExecution();Super::EndPlay(Reason);}
