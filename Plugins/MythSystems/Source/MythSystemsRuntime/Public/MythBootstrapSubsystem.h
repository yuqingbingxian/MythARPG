#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MythBootstrapSubsystem.generated.h"
class UMythChapterCatalog;
class UMythItemCatalog;
class UMythQuestCatalog;
class UMythProgressionRules;
class UMythSkillTreeDefinition;

/** Optional data-only bootstrap. Set soft references in DefaultGame.ini or in Blueprint. */
UCLASS(Config=Game,DefaultConfig)
class MYTHSYSTEMSRUNTIME_API UMythBootstrapSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    UPROPERTY(Config,EditAnywhere,Category="Catalogs") FSoftObjectPath ChapterCatalogPath;
    UPROPERTY(Config,EditAnywhere,Category="Catalogs") FSoftObjectPath ItemCatalogPath;
    UPROPERTY(Config,EditAnywhere,Category="Catalogs") FSoftObjectPath QuestCatalogPath;
    UPROPERTY(Config,EditAnywhere,Category="Catalogs") FSoftObjectPath ProgressionRulesPath;
    UPROPERTY(Config,EditAnywhere,Category="Catalogs") FSoftObjectPath SkillTreePath;
    UFUNCTION(BlueprintCallable) void InitializeCatalogs(UMythChapterCatalog* Chapters,UMythItemCatalog* Items,UMythQuestCatalog* Quests,UMythProgressionRules* Rules,UMythSkillTreeDefinition* Skills);
};
