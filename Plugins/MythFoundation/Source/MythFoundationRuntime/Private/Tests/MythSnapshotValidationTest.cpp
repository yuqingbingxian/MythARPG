#include "Misc/AutomationTest.h"
#include "MythSaveSubsystem.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythSnapshotValidationTest,
    "MythARPG.Foundation.Save.SnapshotValidation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMythSnapshotValidationTest::RunTest(const FString& Parameters)
{
    FMythCampaignSnapshot S;
    TestTrue(TEXT("Clean snapshot accepted"), UMythSaveSubsystem::ValidateSnapshot(S));
    S.CheckpointId = TEXT("CheckpointWithoutChapter");
    TestFalse(TEXT("Checkpoint requires chapter"), UMythSaveSubsystem::ValidateSnapshot(S));
    S.ChapterId = TEXT("SampleChapter");
    TestTrue(TEXT("Valid chapter checkpoint"), UMythSaveSubsystem::ValidateSnapshot(S));
    S.WorldValues.Add(NAME_None, 12);
    TestFalse(TEXT("World flag requires stable ID"), UMythSaveSubsystem::ValidateSnapshot(S));
    S.WorldValues.Empty();
    FMythQuestRecord Quest;
    Quest.QuestId = TEXT("TestQuest");
    Quest.Status = 7;
    S.Quests.Add(Quest);
    TestFalse(TEXT("Invalid quest state"), UMythSaveSubsystem::ValidateSnapshot(S));
    S.Quests[0].Status = 1;
    S.Quests[0].ObjectiveCounts.Add(TEXT("KillCount"), -1);
    TestFalse(TEXT("Negative objective"), UMythSaveSubsystem::ValidateSnapshot(S));
    S.Quests[0].ObjectiveCounts.Empty();
    TestTrue(TEXT("Valid quest state"), UMythSaveSubsystem::ValidateSnapshot(S));
    S.AppliedTransactionIds.Add(NAME_None);
    TestFalse(TEXT("Invalid receipt"), UMythSaveSubsystem::ValidateSnapshot(S));
    return true;
}
#endif
