#include "MythEquipmentComponent.h"
#include "MythInventorySubsystem.h"
#include "MythItemDefinition.h"
#include "MythSaveSubsystem.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
UMythEquipmentComponent::UMythEquipmentComponent(){PrimaryComponentTick.bCanEverTick=false;}
void UMythEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();
    if(GetWorld() && GetWorld()->GetGameInstance())
        if(UMythInventorySubsystem* Inventory=GetWorld()->GetGameInstance()->GetSubsystem<UMythInventorySubsystem>())
            Inventory->OnInventoryUpdated.AddDynamic(this,&UMythEquipmentComponent::OnInventoryChanged);
    // Character BeginPlay must initialize ASC before effects are applied.
    if(GetWorld())GetWorld()->GetTimerManager().SetTimerForNextTick(this,&UMythEquipmentComponent::RefreshEquipment);
}
void UMythEquipmentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if(GetWorld() && GetWorld()->GetGameInstance())
        if(UMythInventorySubsystem* Inventory=GetWorld()->GetGameInstance()->GetSubsystem<UMythInventorySubsystem>())
            Inventory->OnInventoryUpdated.RemoveDynamic(this,&UMythEquipmentComponent::OnInventoryChanged);
    if(IAbilitySystemInterface* Owner=Cast<IAbilitySystemInterface>(GetOwner()))
        if(UAbilitySystemComponent* ASC=Owner->GetAbilitySystemComponent())
            for(const auto& Entry:ActiveEffects)ASC->RemoveActiveGameplayEffect(Entry.Value);
    ActiveEffects.Reset();
    Super::EndPlay(EndPlayReason);
}
void UMythEquipmentComponent::OnInventoryChanged(){RefreshEquipment();}
void UMythEquipmentComponent::RefreshEquipment()
{
    IAbilitySystemInterface* Owner=Cast<IAbilitySystemInterface>(GetOwner());
    UAbilitySystemComponent* ASC=Owner?Owner->GetAbilitySystemComponent():nullptr;
    UGameInstance* GI=GetWorld()?GetWorld()->GetGameInstance():nullptr;
    UMythSaveSubsystem* Save=GI?GI->GetSubsystem<UMythSaveSubsystem>():nullptr;
    UMythInventorySubsystem* Inventory=GI?GI->GetSubsystem<UMythInventorySubsystem>():nullptr;
    if(!ASC || !Save || !Inventory)return;
    for(const auto& Entry:ActiveEffects)ASC->RemoveActiveGameplayEffect(Entry.Value);
    ActiveEffects.Reset();
    const FMythCampaignSnapshot S=Save->GetSnapshot();
    for(const auto& Slot:S.EquippedSlots)
    {
        const FMythItemStack* Item=S.Inventory.FindByPredicate([&Slot](const FMythItemStack& V){return V.InstanceId==Slot.Value;});
        if(!Item)continue;
        const UMythItemDefinition* Def=Inventory->FindDefinition(Item->ItemId);
        if(!Def || Def->EquipSlot!=Slot.Key || !Def->EquippedGameplayEffect)continue;
        FGameplayEffectContextHandle Context=ASC->MakeEffectContext();
        FActiveGameplayEffectHandle Handle=ASC->ApplyGameplayEffectToSelf(
            Def->EquippedGameplayEffect->GetDefaultObject<UGameplayEffect>(),1.f,Context);
        if(Handle.IsValid())ActiveEffects.Add(Slot.Key,Handle);
    }
}
