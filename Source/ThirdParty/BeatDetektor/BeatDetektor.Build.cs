// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class BeatDetektor : ModuleRules
{
	public BeatDetektor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core"
        });
    }
}
