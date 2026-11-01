using UnrealBuildTool;
public class MythARPG : ModuleRules
{
    public MythARPG(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "MythFoundationRuntime", "MythGameplayRuntime", "MythSystemsRuntime"
        });
    }
}
