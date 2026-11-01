#include "Misc/AutomationTest.h"
#include "MythAICombatProfile.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythAIPolicyTest,"MythARPG.Combat.AI.WeightedSelection",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMythAIPolicyTest::RunTest(const FString& Parameters)
{
    UMythAICombatProfile* Profile=NewObject<UMythAICombatProfile>();
    FMythAIAttackOption A;A.AttackId=TEXT("Near");A.MinDistance=0.f;A.MaxDistance=150.f;A.Weight=1.f;
    FMythAIAttackOption B;B.AttackId=TEXT("Far");B.MinDistance=160.f;B.MaxDistance=500.f;B.Weight=1.f;
    Profile->Attacks={A,B};FRandomStream Rng(7);
    const auto* Near=Profile->ChooseAttack(100.f,{},10.f,Rng);
    TestTrue(TEXT("Near attack selected"),Near && Near->AttackId==TEXT("Near"));
    const auto* Far=Profile->ChooseAttack(300.f,{},10.f,Rng);
    TestTrue(TEXT("Far attack selected"),Far && Far->AttackId==TEXT("Far"));
    TMap<FName,double> Cooldown;Cooldown.Add(TEXT("Near"),9.5);
    TestNull(TEXT("Cooldown blocks near attack"),Profile->ChooseAttack(100.f,Cooldown,10.f,Rng));
    TestNull(TEXT("No available range"),Profile->ChooseAttack(155.f,{},10.f,Rng));
    return true;
}
#endif
