using UnrealBuildTool;
public class MythGameplayRuntime : ModuleRules
{
    public MythGameplayRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "MythFoundationRuntime",
            "GameplayAbilities", "GameplayTags", "GameplayTasks", "EnhancedInput", "AIModule", "GameplayStateTreeModule", "StateTreeModule"
        });
        PrivateDependencyModuleNames.AddRange(new string[] {
            "NavigationSystem", "MotionWarping"
        });
    }
}
