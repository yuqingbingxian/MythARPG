#include "MythUIRouterSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/CoreUObjectDelegates.h"

void UMythUIRouterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PreLoadMapHandle=FCoreUObjectDelegates::PreLoadMap.AddUObject(this,&UMythUIRouterSubsystem::OnPreLoadMap);
}
void UMythUIRouterSubsystem::Deinitialize()
{
    FCoreUObjectDelegates::PreLoadMap.Remove(PreLoadMapHandle);
    CloseAll();
    Super::Deinitialize();
}
void UMythUIRouterSubsystem::OnPreLoadMap(const FString& MapName)
{
    // This subsystem survives OpenLevel; widgets and slate focus must not survive their owning world.
    CloseAll();
}
UUserWidget* UMythUIRouterSubsystem::PushWidget(TSubclassOf<UUserWidget> Type,EMythUILayer Layer,bool bPauseGame)
{
    APlayerController* PC=GetGameInstance()?GetGameInstance()->GetFirstLocalPlayerController():nullptr;
    if(!PC || !Type)return nullptr;
    UUserWidget* NewWidget=CreateWidget<UUserWidget>(PC,Type);
    if(!NewWidget)return nullptr;
    FMythWidgetEntry Entry;Entry.Widget=NewWidget;Entry.Layer=Layer;Entry.bPausesGame=bPauseGame;
    const int32 Z=static_cast<int32>(Layer)*1000+Widgets.Num();
    Widgets.Add(Entry);NewWidget->AddToViewport(Z);
    UpdateInputAndPause();OnLayerChanged.Broadcast(Layer);
    return NewWidget;
}
bool UMythUIRouterSubsystem::PopWidget(UUserWidget* Widget)
{
    if(!Widget)return false;
    const int32 Index=Widgets.IndexOfByPredicate([Widget](const FMythWidgetEntry& E){return E.Widget==Widget;});
    if(Index==INDEX_NONE)return false;
    const EMythUILayer Layer=Widgets[Index].Layer;
    Widget->RemoveFromParent();Widgets.RemoveAt(Index);
    UpdateInputAndPause();OnLayerChanged.Broadcast(Layer);return true;
}
void UMythUIRouterSubsystem::ClearLayer(EMythUILayer Layer)
{
    for(int32 I=Widgets.Num()-1;I>=0;--I)
        if(Widgets[I].Layer==Layer)
        {if(Widgets[I].Widget)Widgets[I].Widget->RemoveFromParent();Widgets.RemoveAt(I);}
    UpdateInputAndPause();OnLayerChanged.Broadcast(Layer);
}
void UMythUIRouterSubsystem::CloseAll()
{
    for(FMythWidgetEntry& E:Widgets)if(E.Widget)E.Widget->RemoveFromParent();
    Widgets.Empty();UpdateInputAndPause();
}
UUserWidget* UMythUIRouterSubsystem::GetTopWidget()const
{
    for(int32 L=static_cast<int32>(EMythUILayer::Cinematic);L>=0;--L)
        for(int32 I=Widgets.Num()-1;I>=0;--I)
            if(IsValid(Widgets[I].Widget) && static_cast<int32>(Widgets[I].Layer)==L)return Widgets[I].Widget;
    return nullptr;
}
void UMythUIRouterSubsystem::UpdateInputAndPause()
{
    APlayerController* PC=GetGameInstance()?GetGameInstance()->GetFirstLocalPlayerController():nullptr;
    if(!IsValid(PC))return;
    const bool bWantPause=Widgets.ContainsByPredicate([](const FMythWidgetEntry& E){return E.bPausesGame;});
    if(bWantPause && !bPauseOwned && !UGameplayStatics::IsGamePaused(PC))
    {bPauseOwned=UGameplayStatics::SetGamePaused(PC,true);}
    else if(!bWantPause && bPauseOwned)
    {UGameplayStatics::SetGamePaused(PC,false);bPauseOwned=false;}
    UUserWidget* Top=nullptr;
    for(int32 L=static_cast<int32>(EMythUILayer::Cinematic);L>static_cast<int32>(EMythUILayer::HUD) && !Top;--L)
        for(int32 I=Widgets.Num()-1;I>=0;--I)
            if(IsValid(Widgets[I].Widget) && static_cast<int32>(Widgets[I].Layer)==L)
            {Top=Widgets[I].Widget;break;}
    PC->bShowMouseCursor=Top!=nullptr;
    if(Top)
    {
        FInputModeGameAndUI Mode;
        Mode.SetWidgetToFocus(Top->TakeWidget());
        Mode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(Mode);
    }
    else {FInputModeGameOnly Mode;PC->SetInputMode(Mode);}
}
