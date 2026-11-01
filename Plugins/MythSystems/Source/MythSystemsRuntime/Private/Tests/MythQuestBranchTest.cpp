#include "Misc/AutomationTest.h"
#include "MythQuestDefinition.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythQuestBranchTest,"MythARPG.Systems.Quests.BranchSelection",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMythQuestBranchTest::RunTest(const FString& Parameters)
{
    UMythQuestDefinition* Def=NewObject<UMythQuestDefinition>();
    Def->QuestId=TEXT("TestQuest");Def->InitialStageId=TEXT("Start");
    FMythQuestStage Start;Start.StageId=TEXT("Start");Start.NextStageId=TEXT("Default");
    FMythQuestBranch Branch;Branch.RequiredWorldFlag=TEXT("Choice.Secret");Branch.NextStageId=TEXT("Secret");
    Start.ConditionalBranches.Add(Branch);
    FMythQuestStage Default;Default.StageId=TEXT("Default");
    FMythQuestStage Secret;Secret.StageId=TEXT("Secret");Def->Stages={Start,Default,Secret};
    FString Error;TestTrue(TEXT("Valid branch graph"),Def->ValidateDefinition(Error));
    TMap<FName,int32> Flags;
    TestEqual(TEXT("Fallback branch"),Def->ResolveNextStage(Start,Flags),FName(TEXT("Default")));
    Flags.Add(TEXT("Choice.Secret"),1);
    TestEqual(TEXT("Conditional branch"),Def->ResolveNextStage(Start,Flags),FName(TEXT("Secret")));
    Def->Stages[0].ConditionalBranches[0].NextStageId=TEXT("Missing");
    TestFalse(TEXT("Dangling branch rejected"),Def->ValidateDefinition(Error));
    return true;
}
#endif
