#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythItemDefinition.generated.h"

class UGameplayEffect;
UENUM(BlueprintType)
enum class EMythItemKind : uint8 { Material, Consumable, Weapon, Armor, KeyItem };

/** Identity and rules, not per-player state. Assets and display metadata are optional. */
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName ItemId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FText DisplayName;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) EMythItemKind Kind=EMythItemKind::Material;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin="1")) int32 MaxStack=99;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName EquipSlot=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TSoftObjectPtr<UTexture2D> Icon;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TSubclassOf<UGameplayEffect> EquippedGameplayEffect;
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    { return FPrimaryAssetId(TEXT("Item"),ItemId.IsNone()?GetFName():ItemId); }
    bool IsUnique() const { return Kind==EMythItemKind::Weapon || Kind==EMythItemKind::Armor; }
};

UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythItemCatalog : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<TSoftObjectPtr<UMythItemDefinition>> Items;
    UFUNCTION(BlueprintCallable) UMythItemDefinition* FindItem(FName ItemId) const;
};
