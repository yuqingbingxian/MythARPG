#include "MythLockOnComponent.h"
#include "MythCombatCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

UMythLockOnComponent::UMythLockOnComponent()
{
    PrimaryComponentTick.bCanEverTick=true;
    PrimaryComponentTick.bStartWithTickEnabled=false;
}
bool UMythLockOnComponent::IsCandidate(AMythCombatCharacter* Candidate) const
{
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    if (!Owner || !Candidate || Candidate==Owner || Candidate->IsDead() || Candidate->GetFaction()==Owner->GetFaction()
        || Candidate->GetFaction()==EMythFaction::Neutral || Owner->GetFaction()==EMythFaction::Neutral) return false;
    if (FVector::DistSquared(Owner->GetActorLocation(),Candidate->GetActorLocation())>FMath::Square(MaximumLockedDistance))return false;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(MythTargetLOS),false,Owner);
    FHitResult Hit;
    const bool bBlocked=GetWorld()->LineTraceSingleByChannel(Hit,Owner->GetActorLocation()+FVector(0,0,70),
        Candidate->GetActorLocation()+FVector(0,0,60),ECC_Visibility,Params);
    return !bBlocked || Hit.GetActor()==Candidate;
}
TArray<AMythCombatCharacter*> UMythLockOnComponent::CollectTargets() const
{
    TArray<AMythCombatCharacter*> Results;
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    if (!Owner || !GetWorld()) return Results;
    FCollisionObjectQueryParams Obj;Obj.AddObjectTypesToQuery(ECC_Pawn);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(MythLockOn),false,Owner);
    TArray<FOverlapResult> Overlaps;
    GetWorld()->OverlapMultiByObjectType(Overlaps,Owner->GetActorLocation(),FQuat::Identity,Obj,
        FCollisionShape::MakeSphere(SearchRadius),Params);
    TSet<AMythCombatCharacter*> Unique;
    for (const FOverlapResult& O : Overlaps)
    {
        AMythCombatCharacter* C=Cast<AMythCombatCharacter>(O.GetActor());
        if (C && !Unique.Contains(C) && IsCandidate(C)) {Unique.Add(C);Results.Add(C);}
    }
    return Results;
}
void UMythLockOnComponent::LockTarget(AMythCombatCharacter* NewTarget)
{
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    if (!Owner || !NewTarget) return;
    if (!Target.IsValid())
    {
        bOldControllerYaw=Owner->bUseControllerRotationYaw;
        bOldOrientToMovement=Owner->GetCharacterMovement()->bOrientRotationToMovement;
    }
    Target=NewTarget;
    Owner->bUseControllerRotationYaw=true;
    Owner->GetCharacterMovement()->bOrientRotationToMovement=false;
    SetComponentTickEnabled(true);
}
bool UMythLockOnComponent::ToggleLockOn()
{
    if (Target.IsValid()) {ClearLockOn();return false;}
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    APlayerController* PC=Owner ? Cast<APlayerController>(Owner->GetController()):nullptr;
    if (!PC || Owner->IsDead()) return false;
    const TArray<AMythCombatCharacter*> Candidates=CollectTargets();
    FVector CameraPos; FRotator CameraRot;PC->GetPlayerViewPoint(CameraPos,CameraRot);
    AMythCombatCharacter* Best=nullptr;
    float BestScore=-FLT_MAX;
    for (AMythCombatCharacter* C : Candidates)
    {
        const FVector To=(C->GetActorLocation()-CameraPos).GetSafeNormal();
        const float Dot=FVector::DotProduct(CameraRot.Vector(),To);
        if (Dot<ViewDotMinimum)continue;
        const float Score=Dot*4.f-FVector::Dist(CameraPos,C->GetActorLocation())/SearchRadius;
        if (Score>BestScore){Best=C;BestScore=Score;}
    }
    if (!Best) return false;
    LockTarget(Best);return true;
}
void UMythLockOnComponent::ClearLockOn()
{
    if (AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner()))
    {
        Owner->bUseControllerRotationYaw=bOldControllerYaw;
        Owner->GetCharacterMovement()->bOrientRotationToMovement=bOldOrientToMovement;
    }
    Target.Reset();SetComponentTickEnabled(false);
}
bool UMythLockOnComponent::CycleTarget(bool bRight)
{
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    if (!Owner || !Target.IsValid())return false;
    const FVector CurrentDir=(Target->GetActorLocation()-Owner->GetActorLocation()).GetSafeNormal2D();
    AMythCombatCharacter* Best=nullptr;float BestAngle=10.f;
    for (AMythCombatCharacter* C : CollectTargets())
    {
        if (C==Target.Get())continue;
        const FVector Dir=(C->GetActorLocation()-Owner->GetActorLocation()).GetSafeNormal2D();
        const float Cross=FVector::CrossProduct(CurrentDir,Dir).Z;
        if ((bRight && Cross>=0.f)||(!bRight && Cross<=0.f))continue;
        const float Angle=FMath::Acos(FMath::Clamp(FVector::DotProduct(CurrentDir,Dir),-1.f,1.f));
        if (Angle<BestAngle){BestAngle=Angle;Best=C;}
    }
    if (!Best)return false;
    LockTarget(Best);return true;
}
void UMythLockOnComponent::TickComponent(float DT,ELevelTick TickType,FActorComponentTickFunction* Func)
{
    Super::TickComponent(DT,TickType,Func);
    AMythCombatCharacter* Owner=Cast<AMythCombatCharacter>(GetOwner());
    APlayerController* PC=Owner ? Cast<APlayerController>(Owner->GetController()):nullptr;
    if (!Owner || !PC || Owner->IsDead() || !Target.IsValid() || !IsCandidate(Target.Get())){ClearLockOn();return;}
    FVector CamPos;FRotator CamRot;PC->GetPlayerViewPoint(CamPos,CamRot);
    const FVector Aim=Target->GetActorLocation()+FVector(0,0,65);
    const FRotator Wanted=(Aim-CamPos).Rotation();
    FRotator Smoothed=FMath::RInterpTo(PC->GetControlRotation(),Wanted,DT,CameraRotationSpeed);
    Smoothed.Roll=0.f;
    PC->SetControlRotation(Smoothed);
}
