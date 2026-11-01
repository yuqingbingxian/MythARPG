#include "MythChapterSubsystem.h"
#include "MythChapterCatalog.h"
#include "MythChapterDefinition.h"
#include "MythSaveSubsystem.h"
#include "Engine/GameInstance.h"

void UMythChapterSubsystem::RegisterCatalog(UMythChapterCatalog* InCatalog){Catalog=InCatalog;}

UMythChapterDefinition* UMythChapterSubsystem::FindChapter(FName Id) const
{
    if (!Catalog || Id.IsNone()) return nullptr;
    for (const TSoftObjectPtr<UMythChapterDefinition>& Ref : Catalog->Chapters)
    {
        UMythChapterDefinition* Def=Ref.LoadSynchronous();
        if (Def && Def->ChapterId==Id) return Def;
    }
    return nullptr;
}

bool UMythChapterSubsystem::IsChapterUnlocked(FName Id) const
{
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if (!Save || !FindChapter(Id)) return false;
    const FMythCampaignSnapshot S=Save->GetSnapshot();
    return S.UnlockedChapterIds.Contains(Id) || Id==S.ChapterId;
}

bool UMythChapterSubsystem::UnlockChapter(FName Id)
{
    if (!FindChapter(Id)) return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if (!Save) return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    if (S.UnlockedChapterIds.Contains(Id)) return true;
    S.UnlockedChapterIds.Add(Id);
    return Save->SetSnapshot(S);
}

bool UMythChapterSubsystem::SelectChapter(FName Id,FName Checkpoint)
{
    if (!IsChapterUnlocked(Id)) return false;
    UMythSaveSubsystem* Save=GetGameInstance()->GetSubsystem<UMythSaveSubsystem>();
    if (!Save) return false;
    FMythCampaignSnapshot S=Save->GetSnapshot();
    S.ChapterId=Id;
    S.CheckpointId=Checkpoint;
    S.bHasPlayerState=false; // Prevent spawning at old world's transform.
    if (!Save->SetSnapshot(S,true)) return false;
    OnChapterChanged.Broadcast(Id);
    return true;
}
