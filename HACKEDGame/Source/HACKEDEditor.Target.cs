// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class HACKEDEditorTarget : TargetRules
{
	public HACKEDEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange( new string[] { "HACKED", "JHNET" } );
        bUsesSteam = true;
    }
}
