#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "MythUIRouterSubsystem.generated.h"
UENUM(BlueprintType)
enum class EMythUILayer : uint8 { HUD, Menu, Modal, Cinematic };
USTRUCT()
struct FMythWidgetEntry
{
    GENERATED_BODY()
    UPROPERTY()TObjectPtr<UUserWidget> Widget;
    UPROPERTY()EMythUILayer Layer=EMythUILayer::HUD;
    UPROPERTY()bool bPausesGame=false;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMythUILayerChanged,EMythUILayer,ChangedLayer);
/** Central widget lifetime, focus and local-player menu pause ownership. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythUIRouterSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    UPROPERTY(BlueprintAssignable)FMythUILayerChanged OnLayerChanged;
    UFUNCTION(BlueprintCallable)UUserWidget* PushWidget(TSubclassOf<UUserWidget> Type,EMythUILayer Layer,bool bPauseGame=false);
    UFUNCTION(BlueprintCallable)bool PopWidget(UUserWidget* Widget);
    UFUNCTION(BlueprintCallable)void ClearLayer(EMythUILayer Layer);
    UFUNCTION(BlueprintCallable)void CloseAll();
    UFUNCTION(BlueprintPure)UUserWidget* GetTopWidget()const;
private:
    UPROPERTY(Transient)TArray<FMythWidgetEntry> Widgets;
    bool bPauseOwned=false;
    FDelegateHandle PreLoadMapHandle;
    void OnPreLoadMap(const FString& MapName);
    void UpdateInputAndPause();
};
