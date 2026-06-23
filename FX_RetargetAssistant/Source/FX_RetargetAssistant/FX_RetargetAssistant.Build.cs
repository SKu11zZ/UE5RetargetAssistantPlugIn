using UnrealBuildTool;

public class FX_RetargetAssistant : ModuleRules
{
    public FX_RetargetAssistant(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "AssetRegistry",
                "AssetTools",
                "ContentBrowser",
                "EditorFramework",
                "IKRig",
                "IKRigEditor",
                "Json",
                "JsonUtilities",
                "Projects",
                "PropertyEditor",
                "Slate",
                "SlateCore",
                "ToolMenus",
                "UnrealEd"
            }
        );
    }
}
