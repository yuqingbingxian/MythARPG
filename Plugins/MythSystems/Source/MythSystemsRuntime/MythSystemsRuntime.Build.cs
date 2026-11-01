using UnrealBuildTool;
public class MythSystemsRuntime : ModuleRules
{
    public MythSystemsRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "GameplayTags", "GameplayAbilities",
            "MythFoundationRuntime", "MythGameplayRuntime", "UMG", "Slate", "SlateCore", "EnhancedInput", "InputCore", "LevelSequence", "MovieScene"
        });
    }
}
