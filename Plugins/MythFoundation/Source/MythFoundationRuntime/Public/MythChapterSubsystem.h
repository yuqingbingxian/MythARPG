#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythChapterSubsystem.generated.h"
class UMythChapterCatalog;
class UMythChapterDefinition;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMythChapterChanged,FName,ChapterId);

UCLASS()
class MYTHFOUNDATIONRUNTIME_API UMythChapterSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable,Category="Chapter") FMythChapterChanged OnChapterChanged;
    /** Inject a catalog during game startup; editor content is not required at C++ compile time. */
    UFUNCTION(BlueprintCallable,Category="Chapter") void RegisterCatalog(UMythChapterCatalog* InCatalog);
    UFUNCTION(BlueprintCallable,Category="Chapter") UMythChapterDefinition* FindChapter(FName ChapterId) const;
    UFUNCTION(BlueprintPure,Category="Chapter") bool IsChapterUnlocked(FName ChapterId) const;
    UFUNCTION(BlueprintCallable,Category="Chapter") bool UnlockChapter(FName ChapterId);
    /** Changes progression state, NOT map travel. Map-loader performs travel after validation. */
    UFUNCTION(BlueprintCallable,Category="Chapter") bool SelectChapter(FName ChapterId,FName CheckpointId);
private:
    UPROPERTY(Transient) TObjectPtr<UMythChapterCatalog> Catalog;
};
