// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FrogwaterEditorTarget : TargetRules
{
	public FrogwaterEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("Frogwater");
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
	}
}
