#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythChapterDefinition.generated.h"

class UWorld;

UCLASS(BlueprintType)
class MYTHFOUNDATIONRUNTIME_API UMythChapterDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Chapter")
    FName ChapterId = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Chapter")
    TSoftObjectPtr<UWorld> EntryMap;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Chapter")
    FName EntryCheckpointId = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Chapter")
    TArray<FName> MainQuestIds;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId(TEXT("Chapter"), ChapterId.IsNone() ? GetFName() : ChapterId);
    }
};
