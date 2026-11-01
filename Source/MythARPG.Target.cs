using UnrealBuildTool;
using System.Collections.Generic;
public class MythARPGTarget : TargetRules
{
    public MythARPGTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("MythARPG");
    }
}
