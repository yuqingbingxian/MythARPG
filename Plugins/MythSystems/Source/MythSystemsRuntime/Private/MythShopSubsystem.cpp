#include "MythShopSubsystem.h"
#include "MythShopDefinition.h"
#include "MythSaveSubsystem.h"
#include "MythInventorySubsystem.h"
#include "Engine/GameInstance.h"
bool UMythShopSubsystem::Purchase(const UMythShopDefinition* Shop,FName OfferId)
{
    if(!Shop || Shop->ShopId.IsNone() || OfferId.IsNone())return false;
    const FMythShopOffer* Offer=Shop->Offers.FindByPredicate([OfferId](const FMythShopOffer& O){return O.OfferId==OfferId;});
    if(!Offer || Offer->Count<1 || Offer->Price<0 || Offer->CurrencyId.IsNone())return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    UMythInventorySubsystem* Inv=GetGameInstance()->GetSubsystem<UMythInventorySubsystem>();
    if(!Save || !Inv)return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    const FName CountKey(*(FString(TEXT("Shop."))+Shop->ShopId.ToString()+TEXT(".")+OfferId.ToString()));
    const int32 Purchased=S.WorldValues.FindRef(CountKey);
    if(Offer->PurchaseLimit>0 && Purchased>=Offer->PurchaseLimit)return false;
    const int64 Money=S.Wallet.FindRef(Offer->CurrencyId);
    if(Money<Offer->Price)return false;
    // All changes are staged on one copy. The item check can still fail safely.
    if(!Inv->TryAddToSnapshot(S,Offer->ItemId,Offer->Count))return false;
    S.Wallet.Add(Offer->CurrencyId,Money-Offer->Price);
    if(Purchased==MAX_int32)return false;
    S.WorldValues.Add(CountKey,Purchased+1);
    if(!Save->SetSnapshot(S))return false;
    Inv->NotifyInventoryChanged();
    return true;
}
