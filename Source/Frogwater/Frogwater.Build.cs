// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Frogwater : ModuleRules
{
	public Frogwater(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"HeadMountedDisplay",
			"BasicFootsteps",
			"Niagara",
			"Hermes",
			"Slate",
			"UMG",
			"SlateCore",
			"Water",
			"Landscape"
		});
	}
}
