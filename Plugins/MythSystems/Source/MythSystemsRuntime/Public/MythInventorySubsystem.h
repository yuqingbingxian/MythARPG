#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythCampaignTypes.h"
#include "MythInventorySubsystem.generated.h"
class UMythItemCatalog;
class UMythItemDefinition;
class UMythRecipeDefinition;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMythInventoryUpdated);

/** Single-player inventory, equipment, currency and crafting transactional owner. */
UCLASS()
class MYTHSYSTEMSRUNTIME_API UMythInventorySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable) FMythInventoryUpdated OnInventoryUpdated;
    UFUNCTION(BlueprintCallable) void RegisterCatalog(UMythItemCatalog* Catalog);
    UMythItemDefinition* FindDefinition(FName Id) const;
    UFUNCTION(BlueprintPure) int32 GetCount(FName ItemId) const;
    UFUNCTION(BlueprintPure) int64 GetBalance(FName CurrencyId) const;
    UFUNCTION(BlueprintCallable) bool AddItem(FName ItemId,int32 Count);
    UFUNCTION(BlueprintCallable) bool RemoveItem(FName ItemId,int32 Count);
    UFUNCTION(BlueprintCallable) bool Credit(FName CurrencyId,int64 Amount);
    UFUNCTION(BlueprintCallable) bool Debit(FName CurrencyId,int64 Amount);
    UFUNCTION(BlueprintCallable) bool Equip(FGuid InstanceId,FName Slot);
    UFUNCTION(BlueprintCallable) bool Unequip(FName Slot);
    UFUNCTION(BlueprintCallable) bool Craft(const UMythRecipeDefinition* Recipe);
    // Idempotent content rewards; duplicate rewards with same stable token are ignored.
    UFUNCTION(BlueprintCallable) bool GrantOnce(FName ReceiptId,FName ItemId,int32 Count);
    bool TryAddToSnapshot(FMythCampaignSnapshot& S,FName ItemId,int32 Count) const;
    void NotifyInventoryChanged() {OnInventoryUpdated.Broadcast();}
private:
    UPROPERTY(Transient) TObjectPtr<UMythItemCatalog> ItemCatalog;
    bool ApplySnapshot(const FMythCampaignSnapshot& S);
    bool AddToSnapshot(FMythCampaignSnapshot& S,FName ItemId,int32 Count) const;
    bool RemoveFromSnapshot(FMythCampaignSnapshot& S,FName ItemId,int32 Count) const;
};
