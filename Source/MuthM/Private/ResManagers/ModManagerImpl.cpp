// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ModManagerImpl.h"
#include "FileManager.h"
#include "Paths.h"
#include "PlatformFilemanager.h"
#include "IPlatformFilePak.h"

void UModManagerImpl::InitLoadMods()
{
	if (_ModPlatformFile)
		return;
	TArray<FString> ModsFilename;
	IPlatformFile& InnerPlatform = FPlatformFileManager::Get().GetPlatformFile();
	_ModPlatformFile = new FPakPlatformFile();
	_ModPlatformFile->Initialize(&InnerPlatform, TEXT(""));
	FPlatformFileManager::Get().SetPlatformFile(*_ModPlatformFile);
	IFileManager::Get().FindFiles(ModsFilename, *FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), "/Mods"), TEXT("MMOD"));
	for (int i = 0; i < ModsFilename.Num(); i++)
	{
		LoadModFromPath(ModsFilename[i]);
	}
}

bool UModManagerImpl::LoadModFromPath(FString ModFileName)
{
	if (!_ModPlatformFile)
		return false;
	FPakFile tmpModPak(_ModPlatformFile, *ModFileName, true);
	if (!tmpModPak.IsValid())
		return false;
	tmpModPak.SetMountPoint(*FPaths::ProjectContentDir());
	//XXX:ModMain.uasset? or just ModMain
	//Here we need to search the ModMain class.
	return false;
	//UNDONE: Load Mod From Path
}

void UModManagerImpl::UnloadModByName(FName ModName)
{
	//UNDONE: Unload Mod
}
