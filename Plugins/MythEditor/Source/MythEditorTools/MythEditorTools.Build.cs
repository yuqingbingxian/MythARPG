using UnrealBuildTool;
public class MythEditorTools : ModuleRules
{
    public MythEditorTools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "MythFoundationRuntime", "MythSystemsRuntime" });
        PrivateDependencyModuleNames.AddRange(new string[] {
            "AssetRegistry", "UnrealEd" });
    }
}
