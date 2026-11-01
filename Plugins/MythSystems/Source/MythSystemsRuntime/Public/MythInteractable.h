#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MythInteractable.generated.h"
UINTERFACE(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythInteractable : public UInterface
{
    GENERATED_BODY()
};
class MYTHSYSTEMSRUNTIME_API IMythInteractable
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="Interaction") bool CanInteract(AActor* InstigatorActor) const;
    UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="Interaction") FText GetInteractionLabel(AActor* InstigatorActor) const;
    UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="Interaction") bool ExecuteInteraction(AActor* InstigatorActor);
};
