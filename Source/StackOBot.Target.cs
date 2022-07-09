using System;
using UnrealBuildTool;

public class StackOBotTarget : TargetRules
{
    public StackOBotTarget(TargetInfo Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.V2;
        Type = TargetType.Game;

        ExtraModuleNames.AddRange(new[]
        {
            "StackOBot"
        });
    }
}
