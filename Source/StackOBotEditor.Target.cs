using System;
using UnrealBuildTool;

public class StackOBotEditorTarget : TargetRules
{
    public StackOBotEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

        ExtraModuleNames.AddRange(new[]
        {
            "StackOBotEditor"
        });
    }
}
