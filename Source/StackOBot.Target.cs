using System;
using UnrealBuildTool;

public class StackOBotTarget : TargetRules
{
    public StackOBotTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;

        ExtraModuleNames.AddRange(new[]
        {
            "StackOBot"
        });
        
        
    }
}
