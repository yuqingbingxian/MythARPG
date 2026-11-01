#pragma once
#include "CoreMinimal.h"
#include "Engine/PrimaryDataAsset.h"
#include "MythChapterCatalog.generated.h"
class UMythChapterDefinition;

/** Only add content; common runtime does not hardcode CH01, CH02, ... */
UCLASS(BlueprintType)
class MYTHFOUNDATIONRUNTIME_API UMythChapterCatalog : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Chapters") TArray<TSoftObjectPtr<UMythChapterDefinition>> Chapters;
};
