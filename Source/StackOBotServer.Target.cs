using System;
using UnrealBuildTool;

public class StackOBotServerTarget : TargetRules
{
    public StackOBotServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;

        ExtraModuleNames.AddRange(new[]
        {
            "StackOBot"
        });
    }
}
