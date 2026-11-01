#include "MythInteractionComponent.h"
#include "MythInteractable.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
AActor* UMythInteractionComponent::FindInteractable() const
{
    AActor* Owner=GetOwner();
    if(!Owner || !GetWorld())return nullptr;
    FVector Origin=Owner->GetActorLocation()+FVector(0,0,60);
    FVector Direction=Owner->GetActorForwardVector();
    if(const APawn* Pawn=Cast<APawn>(Owner))
        if(const APlayerController* PC=Cast<APlayerController>(Pawn->GetController()))
            if(PC->PlayerCameraManager){Origin=PC->PlayerCameraManager->GetCameraLocation();Direction=PC->PlayerCameraManager->GetCameraRotation().Vector();}
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(MythInteract),false,Owner);
    if(!GetWorld()->LineTraceSingleByChannel(Hit,Origin,Origin+Direction*FMath::Max(1.f,MaxDistance),ECC_Visibility,Params))return nullptr;
    AActor* Target=Hit.GetActor();
    if(!Target || !Target->GetClass()->ImplementsInterface(UMythInteractable::StaticClass()))return nullptr;
    return IMythInteractable::Execute_CanInteract(Target,Owner)?Target:nullptr;
}
bool UMythInteractionComponent::TryInteract()
{
    AActor* Target=FindInteractable();
    return Target && IMythInteractable::Execute_ExecuteInteraction(Target,GetOwner());
}
