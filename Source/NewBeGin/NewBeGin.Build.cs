// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NewBeGin : ModuleRules
{
	public NewBeGin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"NewBeGin",
			"NewBeGin/Variant_Platforming",
			"NewBeGin/Variant_Platforming/Animation",
			"NewBeGin/Variant_Combat",
			"NewBeGin/Variant_Combat/AI",
			"NewBeGin/Variant_Combat/Animation",
			"NewBeGin/Variant_Combat/Gameplay",
			"NewBeGin/Variant_Combat/Interfaces",
			"NewBeGin/Variant_Combat/UI",
			"NewBeGin/Variant_SideScrolling",
			"NewBeGin/Variant_SideScrolling/AI",
			"NewBeGin/Variant_SideScrolling/Gameplay",
			"NewBeGin/Variant_SideScrolling/Interfaces",
			"NewBeGin/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
