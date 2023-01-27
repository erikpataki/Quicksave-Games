// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FrogwaterTarget : TargetRules
{
	public FrogwaterTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("Frogwater");
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
	}
}
