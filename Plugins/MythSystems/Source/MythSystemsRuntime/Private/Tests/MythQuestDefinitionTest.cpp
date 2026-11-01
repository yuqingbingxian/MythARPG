#include "Misc/AutomationTest.h"
#include "MythQuestDefinition.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythQuestDefinitionTest,"MythARPG.Systems.Quest.Definition",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMythQuestDefinitionTest::RunTest(const FString& Params)
{
    UMythQuestDefinition* Quest=NewObject<UMythQuestDefinition>();
    Quest->QuestId=TEXT("TestQuest");Quest->InitialStageId=TEXT("StageA");
    FMythQuestStage A;A.StageId=TEXT("StageA");A.NextStageId=TEXT("StageB");
    FMythQuestObjective O;O.ObjectiveId=TEXT("Kill");O.EventName=TEXT("EnemyDefeated");O.RequiredCount=2;
    A.Objectives.Add(O);
    FMythQuestStage B;B.StageId=TEXT("StageB");
    Quest->Stages={A,B};
    FString Error;
    TestTrue(TEXT("Valid ordered stages"),Quest->ValidateDefinition(Error));
    Quest->Stages[0].NextStageId=TEXT("Missing");
    TestFalse(TEXT("Broken next-stage link rejected"),Quest->ValidateDefinition(Error));
    return true;
}
#endif
