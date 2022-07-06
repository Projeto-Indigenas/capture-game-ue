using UnrealBuildTool;

public class StackOBotEditor : ModuleRules
{
    public StackOBotEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "NetCore",
            "Engine",
            "StackOBot",
        });
    }
}
