#include "Misc/AutomationTest.h"
#include "MythEncounterDefinition.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythEncounterDefinitionTest,"MythARPG.Systems.Encounter.Validation",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMythEncounterDefinitionTest::RunTest(const FString& Parameters)
{
    UMythEncounterDefinition* D=NewObject<UMythEncounterDefinition>();
    FString Error;
    TestFalse(TEXT("Empty encounter rejected"),D->Validate(Error));
    D->EncounterId=TEXT("Encounter01");
    FMythEncounterWave Wave;Wave.WaveId=TEXT("WaveA");
    D->Waves.Add(Wave);
    TestFalse(TEXT("Wave without enemies rejected"),D->Validate(Error));
    FMythEncounterSpawn Spawn;Spawn.Count=0;
    D->Waves[0].Spawns.Add(Spawn);
    TestFalse(TEXT("Invalid enemy count rejected"),D->Validate(Error));
    return true;
}
#endif
