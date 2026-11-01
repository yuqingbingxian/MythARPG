#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MythContentAuditLibrary.generated.h"

/** Editor Utility Blueprint / commandlet entrypoints. Nothing here enters Shipping builds. */
UCLASS()
class MYTHEDITORTOOLS_API UMythContentAuditLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /** Audit relevant assets within /Game, report issues and return their count. */
    UFUNCTION(BlueprintCallable, Category="Myth|Editor Audit")
    static int32 AuditAllGameAssets(TArray<FString>& OutIssues);

    /** Duplicate/missing persistent IDs within a loaded level. */
    UFUNCTION(BlueprintCallable, Category="Myth|Editor Audit", meta=(WorldContext="WorldContextObject"))
    static int32 AuditWorldPersistentIds(const UObject* WorldContextObject, TArray<FString>& OutIssues);
};
