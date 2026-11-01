#include "Misc/AutomationTest.h"
#include "MythBossPhaseDefinition.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythBossPhaseTest,"MythARPG.Combat.Boss.Phases",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMythBossPhaseTest::RunTest(const FString& Params)
{
    UMythBossPhaseDefinition* Def=NewObject<UMythBossPhaseDefinition>();
    FMythBossPhase A;A.PhaseId=TEXT("A");A.EnterAtOrBelowHealthRatio=1.f;
    FMythBossPhase B;B.PhaseId=TEXT("B");B.EnterAtOrBelowHealthRatio=.5f;
    Def->Phases={A,B};
    FString Error;
    TestTrue(TEXT("Valid phase thresholds"),Def->ValidatePhases(Error));
    TestEqual(TEXT("First phase"),Def->CalculatePhaseIndex(.7f,0),0);
    TestEqual(TEXT("Second phase"),Def->CalculatePhaseIndex(.4f,0),1);
    Def->Phases[1].EnterAtOrBelowHealthRatio=1.f;
    TestFalse(TEXT("Duplicate thresholds rejected"),Def->ValidatePhases(Error));
    return true;
}
#endif
