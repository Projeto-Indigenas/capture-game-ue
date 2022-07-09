using System;
using UnrealBuildTool;

public class StackOBotServerTarget : TargetRules
{
    public StackOBotServerTarget(TargetInfo Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.V2;
        Type = TargetType.Server;

        ExtraModuleNames.AddRange(new[]
        {
            "StackOBot"
        });
    }
}
