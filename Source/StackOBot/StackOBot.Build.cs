using UnrealBuildTool;

public class StackOBot : ModuleRules
{
    public StackOBot(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "NetCore",
            "Engine",
        });
    }
}
