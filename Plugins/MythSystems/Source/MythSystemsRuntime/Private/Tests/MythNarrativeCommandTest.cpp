#include "Misc/AutomationTest.h"
#include "MythNarrativeCommandDefinition.h"
#include "MythNarrativeCommandSubsystem.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythNarrativeCommandTest,"MythARPG.Systems.Narrative.IdempotentCommands",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMythNarrativeCommandTest::RunTest(const FString& Parameters)
{
    UMythNarrativeCommandDefinition* D=NewObject<UMythNarrativeCommandDefinition>();
    D->ReceiptId=TEXT("SEQ.TEST");
    FMythNarrativeCommand C;C.Opcode=EMythNarrativeOpcode::SetWorldValue;
    C.TargetId=TEXT("World.Solved");C.IntValue=1;D->Commands.Add(C);
    FString Error;TestTrue(TEXT("Valid sequence"),D->Validate(Error));
    UMythNarrativeCommandSubsystem* Executor=NewObject<UMythNarrativeCommandSubsystem>();
    FMythCampaignSnapshot Snapshot;
    TestTrue(TEXT("First application"),Executor->ApplyToSnapshot(D,Snapshot));
    TestEqual(TEXT("World flag applied"),Snapshot.WorldValues.FindRef(FName(TEXT("World.Solved"))),1);
    TestFalse(TEXT("One-time receipt blocks duplicate"),Executor->ApplyToSnapshot(D,Snapshot));
    return true;
}
#endif
