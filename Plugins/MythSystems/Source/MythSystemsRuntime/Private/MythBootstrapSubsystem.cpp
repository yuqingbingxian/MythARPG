#include "MythBootstrapSubsystem.h"
#include "MythChapterSubsystem.h"
#include "MythChapterCatalog.h"
#include "MythInventorySubsystem.h"
#include "MythItemDefinition.h"
#include "MythQuestSubsystem.h"
#include "MythQuestDefinition.h"
#include "MythProgressionSubsystem.h"
#include "MythSkillTreeDefinition.h"
#include "MythSaveSubsystem.h"
#include "MythEventSubsystem.h"
#include "Engine/GameInstance.h"
void UMythBootstrapSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UMythSaveSubsystem>();
    Super::Initialize(Collection);
    InitializeCatalogs(
        Cast<UMythChapterCatalog>(ChapterCatalogPath.TryLoad()),
        Cast<UMythItemCatalog>(ItemCatalogPath.TryLoad()),
        Cast<UMythQuestCatalog>(QuestCatalogPath.TryLoad()),
        Cast<UMythProgressionRules>(ProgressionRulesPath.TryLoad()),
        Cast<UMythSkillTreeDefinition>(SkillTreePath.TryLoad()));
}
void UMythBootstrapSubsystem::InitializeCatalogs(UMythChapterCatalog* Chapters,UMythItemCatalog* Items,UMythQuestCatalog* Quests,UMythProgressionRules* Rules,UMythSkillTreeDefinition* Skills)
{
    if(UMythChapterSubsystem* Sub=GetGameInstance()->GetSubsystem<UMythChapterSubsystem>())Sub->RegisterCatalog(Chapters);
    if(UMythInventorySubsystem* Sub=GetGameInstance()->GetSubsystem<UMythInventorySubsystem>())Sub->RegisterCatalog(Items);
    if(UMythQuestSubsystem* Sub=GetGameInstance()->GetSubsystem<UMythQuestSubsystem>())Sub->RegisterCatalog(Quests);
    if(UMythProgressionSubsystem* Sub=GetGameInstance()->GetSubsystem<UMythProgressionSubsystem>()){Sub->RegisterRules(Rules);Sub->RegisterSkillTree(Skills);}
    // All fact consumers must be registered before crash-recovery signal replay.
    if(Quests)
        if(UMythEventSubsystem* Events=GetGameInstance()->GetSubsystem<UMythEventSubsystem>())Events->DeliverPending();
}
