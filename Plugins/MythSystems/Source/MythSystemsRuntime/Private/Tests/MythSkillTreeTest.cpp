#include "Misc/AutomationTest.h"
#include "MythSkillTreeDefinition.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythSkillTreeTest,"MythARPG.Systems.SkillTree.Validation",
    EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FMythSkillTreeTest::RunTest(const FString& Params)
{
    UMythSkillTreeDefinition* Tree=NewObject<UMythSkillTreeDefinition>();
    FMythSkillNode Root;Root.SkillId=TEXT("Root");
    FMythSkillNode Child;Child.SkillId=TEXT("Child");Child.Prerequisites.Add(TEXT("Root"));
    Tree->Nodes={Root,Child};
    FString Error;
    TestTrue(TEXT("Valid tree"),Tree->ValidateTree(Error));
    Tree->Nodes[0].Prerequisites.Add(TEXT("Child"));
    TestFalse(TEXT("Dependency cycle rejected"),Tree->ValidateTree(Error));
    return true;
}
#endif
