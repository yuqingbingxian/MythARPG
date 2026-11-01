#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MythInteractionComponent.generated.h"

/** Single-shot trace; no always-on tick. Gamepad UI calls TryInteract. */
UCLASS(ClassGroup=(Myth),meta=(BlueprintSpawnableComponent))
class MYTHSYSTEMSRUNTIME_API UMythInteractionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Interaction") float MaxDistance=275.f;
    UFUNCTION(BlueprintCallable) AActor* FindInteractable()const;
    UFUNCTION(BlueprintCallable) bool TryInteract();
};
