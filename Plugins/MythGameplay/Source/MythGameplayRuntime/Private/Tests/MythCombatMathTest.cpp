#include "MythCombatMath.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythDamageFormulaTest,
    "MythARPG.Combat.DamageFormula",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMythDamageFormulaTest::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("No defense"), FMythCombatMath::MitigatedDamage(100.f, 0.f), 100.f);
    TestEqual(TEXT("100 defense halves damage"), FMythCombatMath::MitigatedDamage(100.f, 100.f), 50.f);
    TestEqual(TEXT("Negative attack is zero"), FMythCombatMath::MitigatedDamage(-10.f, 0.f), 0.f);
    TestEqual(TEXT("Negative defense is zero"), FMythCombatMath::MitigatedDamage(100.f, -2.f), 100.f);
    return true;
}
#endif
