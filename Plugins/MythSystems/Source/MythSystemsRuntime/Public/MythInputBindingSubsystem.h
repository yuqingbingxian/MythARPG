#pragma once
#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "InputCoreTypes.h"
#include "MythInputBindingSubsystem.generated.h"
class UInputMappingContext;

/** Per-local-player remapping. Never store concrete key names in campaign saves. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythInputBindingSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable,Category="Myth|Input") bool RegisterMappableContext(const UInputMappingContext* Context);
    UFUNCTION(BlueprintCallable,Category="Myth|Input") bool Rebind(FName MappingName,FKey NewKey,int32 SlotIndex=0);
    UFUNCTION(BlueprintCallable,Category="Myth|Input") bool ClearBinding(FName MappingName,int32 SlotIndex=0);
    UFUNCTION(BlueprintCallable,Category="Myth|Input") bool SaveBindings();
    UFUNCTION(BlueprintCallable,Category="Myth|Input") bool ResetBinding(FName MappingName);
};
