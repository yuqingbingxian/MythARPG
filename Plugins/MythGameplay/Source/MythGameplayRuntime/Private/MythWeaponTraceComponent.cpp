#include "MythWeaponTraceComponent.h"
#include "MythCombatCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

UMythWeaponTraceComponent::UMythWeaponTraceComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
    TraceSockets = {TEXT("weapon_base"), TEXT("weapon_mid"), TEXT("weapon_tip")};
}
TArray<FVector> UMythWeaponTraceComponent::SamplePositions() const
{
    TArray<FVector> Locations;
    const AMythCombatCharacter* Owner = Cast<AMythCombatCharacter>(GetOwner());
    if (!Owner) return Locations;
    const USkeletalMeshComponent* Mesh = Owner->GetMesh();
    if (Mesh && Mesh->GetSkeletalMeshAsset())
    {
        for (FName Socket : TraceSockets)
        {
            if (Mesh->DoesSocketExist(Socket)) Locations.Add(Mesh->GetSocketLocation(Socket));
        }
    }
    // Debug-mesh fallback: keep the early source build testable without weapon sockets.
    if (Locations.IsEmpty())
    {
        const FVector Origin = Owner->GetActorLocation() + FVector(0,0,60) + Owner->GetActorForwardVector()*145.f;
        Locations.Add(Origin + FVector(0,0,30));
        Locations.Add(Origin);
        Locations.Add(Origin - FVector(0,0,30));
    }
    return Locations;
}
void UMythWeaponTraceComponent::BeginTrace(float InDamageScale, float InPoiseDamage)
{
    HitActors.Empty();
    DamageScale = FMath::Max(0.f, InDamageScale);
    PoiseDamage = FMath::Max(0.f, InPoiseDamage);
    Previous = SamplePositions();
    bActive = true;
    SetComponentTickEnabled(true);
    // First sample ensures an unmoving dummy can still hit; remaining sweeps follow socket motion.
    for (const FVector& Position : Previous) SweepBetween(Position, Position);
}
void UMythWeaponTraceComponent::EndTrace()
{
    bActive = false;
    SetComponentTickEnabled(false);
    Previous.Reset();
    HitActors.Reset();
}
void UMythWeaponTraceComponent::SweepBetween(const FVector& From, const FVector& To)
{
    AMythCombatCharacter* Source = Cast<AMythCombatCharacter>(GetOwner());
    if (!Source || Source->IsDead() || !GetWorld()) return;
    const float Distance = FVector::Distance(From, To);
    const int32 Steps = FMath::Clamp(FMath::CeilToInt(Distance / FMath::Max(TraceRadius * 2.f, 1.f)),1,MaxSubsteps);
    FVector Prev = From;
    for (int32 Step = 1; Step <= Steps; ++Step)
    {
        const FVector Curr = FMath::Lerp(From,To,static_cast<float>(Step)/Steps);
        FCollisionQueryParams Params(SCENE_QUERY_STAT(MythWeaponTrace), false, Source);
        TArray<FHitResult> Hits;
        GetWorld()->SweepMultiByChannel(Hits,Prev,Curr,FQuat::Identity,ECC_Pawn,
            FCollisionShape::MakeSphere(TraceRadius),Params);
        for (const FHitResult& Hit : Hits)
        {
            AMythCombatCharacter* Target = Cast<AMythCombatCharacter>(Hit.GetActor());
            if (!Target || HitActors.Contains(TWeakObjectPtr<AActor>(Target))) continue;
            HitActors.Add(TWeakObjectPtr<AActor>(Target)); // dedup even if the target is invulnerable
            Source->ApplyMeleeHit(Target, Hit, DamageScale, PoiseDamage);
        }
        Prev=Curr;
    }
}
void UMythWeaponTraceComponent::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* Func)
{
    Super::TickComponent(DeltaSeconds,TickType,Func);
    if (!bActive) return;
    const TArray<FVector> Current = SamplePositions();
    if (Current.Num() == Previous.Num())
    {
        for (int32 Idx=0;Idx<Current.Num();++Idx) SweepBetween(Previous[Idx],Current[Idx]);
    }
    Previous=Current;
}
