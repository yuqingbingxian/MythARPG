#include "MythInventorySubsystem.h"
#include "MythItemDefinition.h"
#include "MythRecipeDefinition.h"
#include "MythSaveSubsystem.h"
#include "Engine/GameInstance.h"

void UMythInventorySubsystem::RegisterCatalog(UMythItemCatalog* Catalog){ItemCatalog=Catalog;}
UMythItemDefinition* UMythInventorySubsystem::FindDefinition(FName Id)const
{return ItemCatalog?ItemCatalog->FindItem(Id):nullptr;}
int32 UMythInventorySubsystem::GetCount(FName Id) const
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if (!Save) return 0;
    int64 Total=0;
    for (const FMythItemStack& Stack:Save->GetSnapshot().Inventory) if(Stack.ItemId==Id) Total+=Stack.Count;
    return static_cast<int32>(FMath::Min<int64>(Total,MAX_int32));
}
int64 UMythInventorySubsystem::GetBalance(FName Currency) const
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    return Save ? Save->GetSnapshot().Wallet.FindRef(Currency):0;
}

bool UMythInventorySubsystem::ApplySnapshot(const FMythCampaignSnapshot& S)
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save || !Save->SetSnapshot(S))return false;
    OnInventoryUpdated.Broadcast();
    return true;
}

bool UMythInventorySubsystem::AddToSnapshot(FMythCampaignSnapshot& S,FName Id,int32 Count) const
{
    if (!ItemCatalog || Id.IsNone() || Count<=0) return false;
    const UMythItemDefinition* Def=ItemCatalog->FindItem(Id);
    if (!Def) return false;
    if (Def->IsUnique())
    {
        // Unique items use one slot + persistent GUID each.
        if (Count>1000) return false;
        for (int32 i=0;i<Count;++i)
        {
            FMythItemStack Stack; Stack.ItemId=Id;Stack.Count=1;Stack.InstanceId=FGuid::NewGuid();
            S.Inventory.Add(Stack);
        }
        return true;
    }
    const int32 MaxStack=FMath::Max(1,Def->MaxStack);
    int32 Remaining=Count;
    for (FMythItemStack& Stack:S.Inventory)
    {
        if(Stack.ItemId!=Id || Stack.InstanceId.IsValid())continue;
        const int32 Room=FMath::Max(0,MaxStack-Stack.Count);
        const int32 Delta=FMath::Min(Remaining,Room);
        Stack.Count+=Delta;Remaining-=Delta;
        if(!Remaining)break;
    }
    while(Remaining>0)
    {
        FMythItemStack Stack;Stack.ItemId=Id;Stack.Count=FMath::Min(Remaining,MaxStack);
        S.Inventory.Add(Stack);Remaining-=Stack.Count;
    }
    return true;
}

bool UMythInventorySubsystem::RemoveFromSnapshot(FMythCampaignSnapshot& S,FName Id,int32 Count) const
{
    if (Id.IsNone() || Count<=0) return false;
    int64 Existing=0;
    for (const FMythItemStack& Item:S.Inventory) if(Item.ItemId==Id) Existing+=Item.Count;
    if (Existing<Count) return false;
    int32 Remaining=Count;
    for(int32 i=S.Inventory.Num()-1;i>=0 && Remaining>0;--i)
    {
        FMythItemStack& Item=S.Inventory[i];
        if(Item.ItemId!=Id) continue;
        const int32 Delta=FMath::Min(Item.Count,Remaining);
        Item.Count-=Delta;Remaining-=Delta;
        if(Item.Count==0)
        {
            if(Item.InstanceId.IsValid())
                for(auto Slot=S.EquippedSlots.CreateIterator();Slot;++Slot)
                    if(Slot.Value()==Item.InstanceId)Slot.RemoveCurrent();
            S.Inventory.RemoveAt(i);
        }
    }
    return Remaining==0;
}

bool UMythInventorySubsystem::AddItem(FName Id,int32 Count)
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    return AddToSnapshot(S,Id,Count)&&ApplySnapshot(S);
}
bool UMythInventorySubsystem::RemoveItem(FName Id,int32 Count)
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    return RemoveFromSnapshot(S,Id,Count)&&ApplySnapshot(S);
}
bool UMythInventorySubsystem::Credit(FName Id,int64 Amount)
{
    if(Id.IsNone() || Amount<=0)return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    const int64 Current=S.Wallet.FindRef(Id);
    if(Amount>MAX_int64-Current)return false;
    S.Wallet.Add(Id,Current+Amount);
    return ApplySnapshot(S);
}
bool UMythInventorySubsystem::Debit(FName Id,int64 Amount)
{
    if(Id.IsNone() || Amount<=0)return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    const int64 Current=S.Wallet.FindRef(Id);
    if(Current<Amount)return false;
    S.Wallet.Add(Id,Current-Amount);
    return ApplySnapshot(S);
}
bool UMythInventorySubsystem::Equip(FGuid Instance,FName Slot)
{
    if(!Instance.IsValid() || Slot.IsNone() || !ItemCatalog)return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    for(const FMythItemStack& Item:S.Inventory)
    {
        if(Item.InstanceId!=Instance)continue;
        const UMythItemDefinition* Def=ItemCatalog->FindItem(Item.ItemId);
        if(!Def || !Def->IsUnique() || Def->EquipSlot!=Slot)return false;
        for(auto It=S.EquippedSlots.CreateIterator();It;++It)
            if(It.Value()==Instance)It.RemoveCurrent();
        S.EquippedSlots.Add(Slot,Instance);
        return ApplySnapshot(S);
    }
    return false;
}
bool UMythInventorySubsystem::Unequip(FName Slot)
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if(S.EquippedSlots.Remove(Slot)==0)return false;
    return ApplySnapshot(S);
}

bool UMythInventorySubsystem::Craft(const UMythRecipeDefinition* Recipe)
{
    if(!Recipe || Recipe->ResultCount<=0 || Recipe->ResultItemId.IsNone())return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    // Mutation only on local copy: all costs are consumed or none are.
    for(const FMythIngredient& Ingredient:Recipe->Ingredients)
        if(!RemoveFromSnapshot(S,Ingredient.ItemId,Ingredient.Count))return false;
    return AddToSnapshot(S,Recipe->ResultItemId,Recipe->ResultCount)&&ApplySnapshot(S);
}

bool UMythInventorySubsystem::GrantOnce(FName Receipt,FName Id,int32 Count)
{
    if(Receipt.IsNone())return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if(!Save)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if(S.AppliedTransactionIds.Contains(Receipt))return true;
    if(!AddToSnapshot(S,Id,Count))return false;
    S.AppliedTransactionIds.Add(Receipt);
    return ApplySnapshot(S);
}

bool UMythInventorySubsystem::TryAddToSnapshot(FMythCampaignSnapshot& S,FName Id,int32 Count)const
{return AddToSnapshot(S,Id,Count);}
