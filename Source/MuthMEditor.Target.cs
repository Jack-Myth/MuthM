// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class MuthMEditorTarget : TargetRules
{
	public MuthMEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
        BuildEnvironment = TargetBuildEnvironment.Shared;
        ExtraModuleNames.AddRange( new string[] { "MuthM","GameHAL" } );
	}
}
