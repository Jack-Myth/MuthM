// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class libminimp3 : ModuleRules
{
	public libminimp3(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        		PublicIncludePaths.Add(Target.UEThirdPartySourceDirectory+"libminimp3");
    	}
}
