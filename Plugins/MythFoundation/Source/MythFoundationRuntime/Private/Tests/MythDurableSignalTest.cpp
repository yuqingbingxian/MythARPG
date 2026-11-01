#include "MythSaveSubsystem.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythDurableOutboxTest,"Myth.Foundation.DurableOutboxValidation",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMythDurableOutboxTest::RunTest(const FString& Parameters)
{
    FMythCampaignSnapshot S;
    FMythQueuedSignal E;E.ReceiptId=TEXT("Quest01.Stage1.Event0");
    E.EventName=TEXT("QuestObjective");E.Count=1;
    S.PendingSignals.Add(E);
    TestTrue(TEXT("One pending event is valid"),UMythSaveSubsystem::ValidateSnapshot(S));
    S.PendingSignals.Add(E);
    TestFalse(TEXT("Duplicate receipts rejected"),UMythSaveSubsystem::ValidateSnapshot(S));
    S.PendingSignals.Pop();S.PendingSignals[0].Count=-1;
    TestFalse(TEXT("Invalid count rejected"),UMythSaveSubsystem::ValidateSnapshot(S));
    S.PendingSignals.Reset();S.TravelRecovery.bPending=true;
    TestFalse(TEXT("Pending travel requires previous chapter"),UMythSaveSubsystem::ValidateSnapshot(S));
    S.TravelRecovery.PreviousChapterId=TEXT("CH01");
    TestTrue(TEXT("Valid recovery marker"),UMythSaveSubsystem::ValidateSnapshot(S));
    return true;
}
#endif
