// Copyright (C) 2018 JackMyth. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class GameHAL : ModuleRules
{
	public GameHAL(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Launch",
            "InputCore" });

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));

        PrivateDependencyModuleNames.AddRange(new string[] {  });

        if (Target.IsInPlatformGroup(UnrealPlatformGroup.Android))
        {
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "/Private/AndroidUPL.xml"));
        }

		// Uncomment if you are using Slate UI
		//PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore","UMG"});

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
