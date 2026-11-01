#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythGameFlowSubsystem.generated.h"
UENUM(BlueprintType)
enum class EMythGameScreenState : uint8 { MainMenu, Exploring, Dialogue, Cinematic, Paused, Loading, Dead };
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMythScreenStateChanged,EMythGameScreenState,OldState,EMythGameScreenState,NewState);
/** Orchestrates transitions, not individual widgets / level sequences. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythGameFlowSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable) FMythScreenStateChanged OnScreenStateChanged;
    UFUNCTION(BlueprintPure) EMythGameScreenState GetScreenState()const{return ScreenState;}
    UFUNCTION(BlueprintCallable) bool TryChangeScreenState(EMythGameScreenState Next);
private:
    EMythGameScreenState ScreenState=EMythGameScreenState::MainMenu;
};
