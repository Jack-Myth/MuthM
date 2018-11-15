// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class libmpg123: ModuleRules
{
	public libmpg123(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateIncludePathModuleNames.AddRange(new string[] {"libmpg123"});
        PrivateDefinitions.Add("OPT_MULTI");
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PrivateDefinitions.Add("OPT_X86_64");
        }
        else if(Target.Platform == UnrealTargetPlatform.Win32)
        {
            PrivateDefinitions.Add("OPT_X86");
        }
        else if (Target.Platform == UnrealTargetPlatform.Android|| Target.Platform == UnrealTargetPlatform.IOS)
        {
            PrivateDefinitions.Add("OPT_ARM");
        }
    }
}
