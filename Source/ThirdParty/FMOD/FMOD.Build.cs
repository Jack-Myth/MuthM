// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class FMOD : ModuleRules
{
    public FMOD(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));
        string configName = "";

        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            configName = "L";
            PublicDefinitions.Add("FMODSTUDIO_LINK_LOGGING=1");
        }
        else
        {
            configName = "";
            PublicDefinitions.Add("FMODSTUDIO_LINK_RELEASE=1");
        }
        string platformName = Target.Platform.ToString();

        string platformMidName = "";
        string linkExtension = "";
        string dllExtension = "";
        string libPrefix = "";

        // ModuleDirectory points to FMOD\Lib, need to get back to binaries directory for our libs
        string BasePath = System.IO.Path.Combine(ModuleDirectory, "Lib", platformName);
        // Collapse the directory path, otherwise OSX is having issues with plugin paths.
        BasePath = Utils.CleanDirectorySeparators(BasePath);

        string copyThirdPartyPath = "";
        bool bAddRuntimeDependencies = true;
        bool bAddDelayLoad = false;
        bool bShortLinkNames = false;
        bool bLinkFromBinaries = true;

        // Minimum UE version for Switch 4.15
        System.Console.WriteLine("Target Platform -- " + Target.Platform.ToString());
        if (Target.Platform.ToString() == "Switch")
        {
            linkExtension = ".a";
            dllExtension = ".a";
            libPrefix = "lib";
            bAddRuntimeDependencies = false;
        }
        else if (Target.Platform.ToString() == "UWP64")
        {
            platformMidName = "_X64";
            linkExtension = ".lib";
            dllExtension = ".dll";
            bAddDelayLoad = true;
        }
        else
        {
            switch (Target.Platform)
            {
                case UnrealTargetPlatform.Win32:
                    linkExtension = "_vc.lib";
                    dllExtension = ".dll";
                    bAddDelayLoad = true;
                    break;
                case UnrealTargetPlatform.Win64:
                    platformMidName = "64";
                    linkExtension = "_vc.lib";
                    dllExtension = ".dll";
                    bAddDelayLoad = true;
                    break;
                case UnrealTargetPlatform.Mac:
                    linkExtension = dllExtension = ".dylib";
                    libPrefix = "lib";
                    bLinkFromBinaries = false;
                    break;
                case UnrealTargetPlatform.XboxOne:
                    linkExtension = "_vc.lib";
                    dllExtension = ".dll";
                    copyThirdPartyPath = "../XBoxOne"; // XBoxOne still doesn't seem to support plugins with .dlls
                    bAddDelayLoad = false;
                    break;
                case UnrealTargetPlatform.PS4:
                    linkExtension = "_stub.a";
                    dllExtension = ".prx";
                    libPrefix = "lib";
                    bAddDelayLoad = true;
                    break;
                case UnrealTargetPlatform.Android:
                    // Don't use an explicit path with the .so, let the architecture dirs be filtered by UBT
                    PublicLibraryPaths.Add(System.IO.Path.Combine(BasePath, "armeabi-v7a"));
                    PublicLibraryPaths.Add(System.IO.Path.Combine(BasePath, "arm64-v8a"));
                    PublicLibraryPaths.Add(System.IO.Path.Combine(BasePath, "x86"));
                    bAddRuntimeDependencies = false; // Don't use this system
                    bShortLinkNames = true; // strip off lib and .so
                    linkExtension = dllExtension = ".so";
                    libPrefix = "lib";
                    break;
                case UnrealTargetPlatform.IOS:
                    linkExtension = "_iphoneos.a";
                    libPrefix = "lib";
                    bAddRuntimeDependencies = false;
                    break;
                case UnrealTargetPlatform.Linux:
                    BasePath = System.IO.Path.Combine(BasePath, "x86_64");
                    linkExtension = ".so";
                    dllExtension = ".so";
                    libPrefix = "lib";
                    break;
                default:
                    //extName = ".a";
                    throw new System.Exception(System.String.Format("Unsupported platform {0}", Target.Platform.ToString()));
                    //break;
            }
        }
        PublicLibraryPaths.Add(BasePath);

        string fmodLibName = System.String.Format("{0}fmod{1}{2}{3}", libPrefix, configName, platformMidName, linkExtension);

        string fmodDllName = System.String.Format("{0}fmod{1}{2}{3}", libPrefix, configName, platformMidName, dllExtension);

        string fmodLibPath = System.IO.Path.Combine(BasePath, fmodLibName);

        string fmodDllPath = System.IO.Path.Combine(BasePath, fmodDllName);

        System.Collections.Generic.List<string> plugins = GetPlugins(BasePath);

        if (bShortLinkNames)
        {
            // For android we have provided the paths to all architectures above
            // Just provide the name without "lib" and without extension
            PublicAdditionalLibraries.Add(System.String.Format("fmod{0}{1}", configName, platformMidName));
        }
        else if (bLinkFromBinaries)
        {
            PublicAdditionalLibraries.Add(fmodLibPath);
        }
        else
        {
            string LibPath = System.IO.Path.Combine(ModuleDirectory, "../../Libs/Mac/");
            PublicAdditionalLibraries.Add(System.String.Format("{0}libfmod{1}.dylib", LibPath, configName));
        }

        if (bAddRuntimeDependencies)
        {
            RuntimeDependencies.Add(fmodDllPath);
            foreach (string plugin in plugins)
            {
                string pluginPath = System.IO.Path.Combine(BasePath, plugin + dllExtension);
                System.Console.WriteLine("Adding reference to FMOD plugin: " + pluginPath);
                RuntimeDependencies.Add(pluginPath);
            }
        }

        if (copyThirdPartyPath.Length != 0)
        {
            string destPath = System.IO.Path.Combine(Target.UEThirdPartyBinariesDirectory, copyThirdPartyPath);
            System.IO.Directory.CreateDirectory(destPath);

            string fmodDllDest = System.IO.Path.Combine(destPath, fmodDllName);

            CopyFile(fmodDllPath, fmodDllDest);
        }

        if (bAddDelayLoad)
        {
            PublicDelayLoadDLLs.AddRange(
                    new string[] {
                            fmodDllName
                        }
                    );
        }

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string APLName = System.String.Format("FMOD{0}_APL.xml", configName);
            string RelAPLPath = Utils.MakePathRelativeTo(System.IO.Path.Combine(ModuleDirectory, APLName), Target.RelativeEnginePath);
            System.Console.WriteLine("Adding {0}", RelAPLPath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", RelAPLPath);
            foreach (string PluginName in System.IO.Directory.GetFiles(BasePath))
            {
                if (PluginName.EndsWith("_APL.xml", System.StringComparison.OrdinalIgnoreCase))
                {
                    string RelPluginPath = Utils.MakePathRelativeTo(PluginName, Target.RelativeEnginePath);
                    System.Console.WriteLine("Adding {0}", RelPluginPath);
                    AdditionalPropertiesForReceipt.Add("AndroidPlugin", RelPluginPath);
                }
            }
        }
    }

    private void CopyFile(string source, string dest)
    {
        //System.Console.WriteLine("Copying {0} to {1}", source, dest);
        if (System.IO.File.Exists(dest))
        {
            System.IO.File.SetAttributes(dest, System.IO.File.GetAttributes(dest) & ~System.IO.FileAttributes.ReadOnly);
        }
        try
        {
            System.IO.File.Copy(source, dest, true);
        }
        catch (System.Exception ex)
        {
            System.Console.WriteLine("Failed to copy file: {0}", ex.Message);
        }
    }

    private System.Collections.Generic.List<string> GetPlugins(string BasePath)
    {
        System.Collections.Generic.List<string> AllPlugins = new System.Collections.Generic.List<string>();
        string PluginListName = System.IO.Path.Combine(BasePath, "plugins.txt");
        if (System.IO.File.Exists(PluginListName))
        {
            try
            {
                foreach (string FullEntry in System.IO.File.ReadAllLines(PluginListName))
                {
                    string Entry = FullEntry.Trim();
                    if (Entry.Length > 0)
                    {
                        AllPlugins.Add(Entry);
                    }
                }
            }
            catch (System.Exception ex)
            {
                System.Console.WriteLine("Failed to read plugin list file: {0}", ex.Message);
            }
        }
        return AllPlugins;
    }
}
