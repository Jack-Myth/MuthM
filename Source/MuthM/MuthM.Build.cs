// Copyright (C) 2018 JackMyth. All Rights Reserved.

using UnrealBuildTool;

public class MuthM : ModuleRules
{
	public MuthM(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "zlib",
            "Vorbis",
            "VorbisFile",
            "UEOgg",
            "libminimp3",
            "AudioPlatformConfiguration",
            "BeatDetektor",
            "Kiss_FFT",
            "Json",
            "PakFile",
            "HTTP",
            "GameHAL",
            "BlueprintJson" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore","UMG"});

        if(Target.IsInPlatformGroup(UnrealPlatformGroup.Android))
        {
            PublicDefinitions.Add("_LARGEFILE64_SOURCE=0");
            PublicDefinitions.Add("_FILE_OFFSET_BITS=0");
        }

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
