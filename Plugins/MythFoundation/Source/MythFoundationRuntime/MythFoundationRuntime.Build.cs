using UnrealBuildTool;
public class MythFoundationRuntime : ModuleRules
{
    public MythFoundationRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "GameplayTags" });
    }
}
