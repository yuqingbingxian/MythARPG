#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythShopDefinition.generated.h"
USTRUCT(BlueprintType)
struct FMythShopOffer
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName OfferId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName ItemId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="1")) int32 Count=1;
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName CurrencyId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="0")) int64 Price=0;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="0")) int32 PurchaseLimit=0;
};
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythShopDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName ShopId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythShopOffer> Offers;
};
