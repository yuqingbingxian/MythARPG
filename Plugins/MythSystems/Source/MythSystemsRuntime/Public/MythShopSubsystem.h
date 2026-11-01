#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythShopSubsystem.generated.h"
class UMythShopDefinition;
/** Atomic purchase against the campaign snapshot. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythShopSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable) bool Purchase(const UMythShopDefinition* Shop,FName OfferId);
};
