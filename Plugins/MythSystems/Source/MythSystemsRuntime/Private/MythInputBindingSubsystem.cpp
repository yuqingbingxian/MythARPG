#include "MythInputBindingSubsystem.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"

static UEnhancedInputUserSettings* GetMythUserSettings(ULocalPlayer* Player)
{
    UEnhancedInputLocalPlayerSubsystem* Sub=Player?Player->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>():nullptr;
    return Sub?Sub->GetUserSettings():nullptr;
}
bool UMythInputBindingSubsystem::RegisterMappableContext(const UInputMappingContext* Context)
{
    UEnhancedInputUserSettings* Settings=GetMythUserSettings(GetLocalPlayer());
    return Settings && Context && Settings->RegisterInputMappingContext(Context);
}
bool UMythInputBindingSubsystem::Rebind(FName MappingName,FKey NewKey,int32 SlotIndex)
{
    UEnhancedInputUserSettings* Settings=GetMythUserSettings(GetLocalPlayer());
    if(!Settings || MappingName.IsNone() || !NewKey.IsValid() || SlotIndex<0 || SlotIndex>1)return false;
    FMapPlayerKeyArgs Args;Args.MappingName=MappingName;Args.NewKey=NewKey;
    Args.Slot=SlotIndex==0?EPlayerMappableKeySlot::First:EPlayerMappableKeySlot::Second;
    FGameplayTagContainer Failures;
    Settings->MapPlayerKey(Args,Failures);
    if(!Failures.IsEmpty())return false;
    Settings->ApplySettings();
    Settings->SaveSettings();
    return true;
}
bool UMythInputBindingSubsystem::ClearBinding(FName MappingName,int32 SlotIndex)
{
    UEnhancedInputUserSettings* Settings=GetMythUserSettings(GetLocalPlayer());
    if(!Settings || MappingName.IsNone() || SlotIndex<0 || SlotIndex>1)return false;
    FMapPlayerKeyArgs Args;Args.MappingName=MappingName;
    Args.Slot=SlotIndex==0?EPlayerMappableKeySlot::First:EPlayerMappableKeySlot::Second;
    FGameplayTagContainer Failures;Settings->UnMapPlayerKey(Args,Failures);
    if(!Failures.IsEmpty())return false;
    Settings->ApplySettings();Settings->SaveSettings();return true;
}
bool UMythInputBindingSubsystem::SaveBindings()
{
    if(UEnhancedInputUserSettings* Settings=GetMythUserSettings(GetLocalPlayer()))
    {Settings->SaveSettings();return true;}
    return false;
}
bool UMythInputBindingSubsystem::ResetBinding(FName MappingName)
{
    UEnhancedInputUserSettings* Settings=GetMythUserSettings(GetLocalPlayer());
    if(!Settings || MappingName.IsNone())return false;
    FMapPlayerKeyArgs Args;Args.MappingName=MappingName;
    FGameplayTagContainer Failures;
    Settings->ResetAllPlayerKeysInRow(Args,Failures);
    if(!Failures.IsEmpty())return false;
    Settings->ApplySettings();Settings->SaveSettings();return true;
}
