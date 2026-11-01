#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MythHUDViewModel.generated.h"
class AMythCombatCharacter;
struct FOnAttributeChangeData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMythHUDStatsChanged);
/** Read-only adapter: widgets bind to this, never write game attributes directly. */
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythHUDViewModel : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)FMythHUDStatsChanged OnStatsChanged;
    UFUNCTION(BlueprintCallable)void BindCombatant(AMythCombatCharacter* Combatant);
    UFUNCTION(BlueprintCallable)void UnbindCombatant();
    UFUNCTION(BlueprintPure)float HealthFraction()const;
    UFUNCTION(BlueprintPure)float StaminaFraction()const;
    UFUNCTION(BlueprintPure)float PoiseFraction()const;
    UFUNCTION(BlueprintPure)bool IsCharacterDead()const;
    virtual void BeginDestroy() override;
private:
    UPROPERTY(Transient)TWeakObjectPtr<AMythCombatCharacter> Subject;
    void OnAttributeChanged(const FOnAttributeChangeData& Data);
};
