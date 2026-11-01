#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "MythComboGraph.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythComboGraphTest,
    "MythARPG.Combat.ComboGraph.Transitions",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMythComboGraphTest::RunTest(const FString& Parameters)
{
    UMythComboGraph* Graph=NewObject<UMythComboGraph>();
    FMythComboNode A;A.NodeId=TEXT("Light1");A.EntryInput=EMythAttackInput::Light;
    FMythComboNode B;B.NodeId=TEXT("HeavyEnd");B.EntryInput=EMythAttackInput::Heavy;
    Graph->Nodes={A,B};Graph->EntryLight=A.NodeId;Graph->EntryHeavy=B.NodeId;
    FMythComboLink Link;Link.From=A.NodeId;Link.Input=EMythAttackInput::Heavy;Link.To=B.NodeId;
    Graph->Links.Add(Link);
    FString Error;
    TestTrue(TEXT("Valid graph"),Graph->ValidateGraph(Error));
    TestEqual(TEXT("Light entry"),Graph->FindEntry(EMythAttackInput::Light)->NodeId,A.NodeId);
    TestEqual(TEXT("Heavy branch"),Graph->FindNext(A.NodeId,EMythAttackInput::Heavy)->NodeId,B.NodeId);
    TestTrue(TEXT("Unknown link rejected"),Graph->FindNext(B.NodeId,EMythAttackInput::Light)==nullptr);
    Graph->Links.Add(Link);
    TestFalse(TEXT("Duplicate link rejected"),Graph->ValidateGraph(Error));
    return true;
}
#endif
