#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "MythEquipmentComponent.generated.h"

/** Bridges persistent equipped item IDs to GAS effects on the owning combatant. */
UCLASS(ClassGroup=(Myth),meta=(BlueprintSpawnableComponent))
class MYTHSYSTEMSRUNTIME_API UMythEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UMythEquipmentComponent();
    UFUNCTION(BlueprintCallable) void RefreshEquipment();
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
    UFUNCTION() void OnInventoryChanged();
    TMap<FName,FActiveGameplayEffectHandle> ActiveEffects;
};
