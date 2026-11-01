#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythRecipeDefinition.generated.h"
USTRUCT(BlueprintType)
struct FMythIngredient
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadOnly) FName ItemId=NAME_None;
    UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(ClampMin="1")) int32 Count=1;
};
UCLASS(BlueprintType)
class MYTHSYSTEMSRUNTIME_API UMythRecipeDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName RecipeId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) TArray<FMythIngredient> Ingredients;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly) FName ResultItemId=NAME_None;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(ClampMin="1")) int32 ResultCount=1;
};
