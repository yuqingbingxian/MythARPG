#include "MythItemDefinition.h"
UMythItemDefinition* UMythItemCatalog::FindItem(FName ItemId) const
{
    if (ItemId.IsNone()) return nullptr;
    for (const TSoftObjectPtr<UMythItemDefinition>& Ref:Items)
    {
        UMythItemDefinition* D=Ref.LoadSynchronous();
        if (D && D->ItemId==ItemId) return D;
    }
    return nullptr;
}
