using System;
using UnrealBuildTool;

public class StackOBotEditorTarget : TargetRules
{
    public StackOBotEditorTarget(TargetInfo Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.V2;
        Type = TargetType.Editor;

        ExtraModuleNames.AddRange(new[]
        {
            "StackOBotEditor"
        });
    }
}
