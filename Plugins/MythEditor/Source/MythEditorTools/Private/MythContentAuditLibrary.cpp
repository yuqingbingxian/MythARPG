#include "MythContentAuditLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "MythChapterCatalog.h"
#include "MythChapterDefinition.h"
#include "MythQuestDefinition.h"
#include "MythItemDefinition.h"
#include "MythDialogueDefinition.h"
#include "MythSkillTreeDefinition.h"
#include "MythPersistentWorldComponent.h"

namespace
{
    void Issue(TArray<FString>& Problems, const UObject* Asset, const FString& Message)
    {
        Problems.Add(FString::Printf(TEXT("%s: %s"), *GetNameSafe(Asset), *Message));
    }
    void CheckDialogue(const UMythDialogueDefinition* D, TArray<FString>& Out)
    {
        if (D->DialogueId.IsNone() || D->StartNodeId.IsNone() || !D->HasNode(D->StartNodeId))
            Issue(Out, D, TEXT("Missing dialogue ID or valid start node"));
        TSet<FName> Seen;
        for (const FMythDialogueNode& Node : D->Nodes)
        {
            if (Node.NodeId.IsNone() || Seen.Contains(Node.NodeId))
                Issue(Out, D, TEXT("Duplicate/empty dialogue node ID"));
            Seen.Add(Node.NodeId);
            for (const FMythDialogueChoice& Choice : Node.Choices)
                if (!Choice.NextNodeId.IsNone() && !D->HasNode(Choice.NextNodeId))
                    Issue(Out, D, FString::Printf(TEXT("Missing target of choice from %s"), *Node.NodeId.ToString()));
        }
    }
    void CheckChapterCatalog(UMythChapterCatalog* Catalog, TArray<FString>& Out)
    {
        TSet<FName> Seen;
        for (const auto& Ref : Catalog->Chapters)
        {
            UMythChapterDefinition* Chapter = Ref.LoadSynchronous();
            if (!Chapter) { Issue(Out, Catalog, TEXT("Null chapter reference")); continue; }
            if (Chapter->ChapterId.IsNone() || Seen.Contains(Chapter->ChapterId))
                Issue(Out, Catalog, TEXT("Duplicate or empty chapter ID"));
            if (Chapter->EntryMap.IsNull()) Issue(Out, Chapter, TEXT("Chapter map is unassigned"));
            Seen.Add(Chapter->ChapterId);
        }
    }
    void CheckItemCatalog(UMythItemCatalog* Catalog, TArray<FString>& Out)
    {
        TSet<FName> Seen;
        for (const auto& Ref : Catalog->Items)
        {
            UMythItemDefinition* Item = Ref.LoadSynchronous();
            if (!Item) { Issue(Out, Catalog, TEXT("Null item reference")); continue; }
            if (Item->ItemId.IsNone() || Seen.Contains(Item->ItemId))
                Issue(Out, Catalog, TEXT("Duplicate/empty item ID"));
            if (Item->MaxStack < 1 || (Item->IsUnique() && Item->MaxStack > 1))
                Issue(Out, Item, TEXT("Invalid stack rule for item type"));
            Seen.Add(Item->ItemId);
        }
    }
}

int32 UMythContentAuditLibrary::AuditAllGameAssets(TArray<FString>& OutIssues)
{
    OutIssues.Reset();
    FAssetRegistryModule& RegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    TArray<FAssetData> Assets;
    RegistryModule.Get().GetAssetsByPath(FName(TEXT("/Game")), Assets, true);
    for (const FAssetData& AssetData : Assets)
    {
        // Check asset class name before loading to avoid loading every large map/mesh.
        const FName ClassName = AssetData.AssetClassPath.GetAssetName();
        const bool bRelevant = ClassName == UMythChapterCatalog::StaticClass()->GetFName() ||
            ClassName == UMythItemCatalog::StaticClass()->GetFName() ||
            ClassName == UMythQuestDefinition::StaticClass()->GetFName() ||
            ClassName == UMythSkillTreeDefinition::StaticClass()->GetFName() ||
            ClassName == UMythDialogueDefinition::StaticClass()->GetFName();
        if (!bRelevant) continue;
        UObject* Asset = AssetData.GetAsset();
        if (!Asset) { OutIssues.Add(AssetData.AssetName.ToString() + TEXT(": could not load")); continue; }
        if (auto* Catalog = Cast<UMythChapterCatalog>(Asset)) CheckChapterCatalog(Catalog, OutIssues);
        if (auto* Catalog = Cast<UMythItemCatalog>(Asset)) CheckItemCatalog(Catalog, OutIssues);
        if (auto* Quest = Cast<UMythQuestDefinition>(Asset))
        {
            FString Message;
            if (!Quest->ValidateDefinition(Message)) Issue(OutIssues, Quest, Message);
        }
        if (auto* Tree = Cast<UMythSkillTreeDefinition>(Asset))
        {
            FString Message;
            if (!Tree->ValidateTree(Message)) Issue(OutIssues, Tree, Message);
        }
        if (auto* Dialogue = Cast<UMythDialogueDefinition>(Asset)) CheckDialogue(Dialogue, OutIssues);
    }
    for (const FString& Problem : OutIssues)
        UE_LOG(LogTemp, Error, TEXT("MythARPG content audit: %s"), *Problem);
    return OutIssues.Num();
}

int32 UMythContentAuditLibrary::AuditWorldPersistentIds(const UObject* WorldContextObject, TArray<FString>& OutIssues)
{
    OutIssues.Reset();
    UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
    if (!World) { OutIssues.Add(TEXT("No valid world for persistent ID audit")); return 1; }
    TSet<FName> Seen;
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        TInlineComponentArray<UMythPersistentWorldComponent*> Components;
        It->GetComponents(Components);
        for (const UMythPersistentWorldComponent* Component : Components)
        {
            if (!Component || Component->PersistentId.IsNone())
                OutIssues.Add(It->GetPathName() + TEXT(": Missing persistent ID"));
            else if (Seen.Contains(Component->PersistentId))
                OutIssues.Add(It->GetPathName() + TEXT(": Duplicate persistent ID: ") + Component->PersistentId.ToString());
            else Seen.Add(Component->PersistentId);
        }
    }
    return OutIssues.Num();
}
