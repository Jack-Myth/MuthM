// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ModManagerImpl.h"
#include "FileManager.h"
#include "Paths.h"
#include "PlatformFilemanager.h"
#include "IPlatformFilePak.h"
#include "FileHelper.h"
#include "JsonObject.h"
#include "MuthMBPLib.h"
#include "MuthMGameInstance.h"
#include "Engine/World.h"
#include "ModInstanceBase.h"
#include "PackageName.h"
#include "CoreDelegates.h"

DEFINE_LOG_CATEGORY(MuthMModManager)

void UModManagerImpl::RefreshModsList()
{
	ModsList.Empty();
	TArray<FString> ModsFilename;
	IFileManager::Get().FindFiles(ModsFilename, *FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("Mods")), TEXT("minfo"));
	auto SaveGame = Cast<UMuthMGameInstance>(GetWorld()->GetGameInstance())->GetGlobalSaveGame();
	for (int i = 0; i < ModsFilename.Num(); i++)
	{
		FString ModInfoStr;
		TSharedPtr<FJsonObject> ModInfoJson;
		if (!FFileHelper::LoadFileToString(ModInfoStr, *FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("Mods"),ModsFilename[i])) ||
			!(ModInfoJson = UMuthMBPLib::DeserializeJsonFromStr(ModInfoStr)))
		{
			UE_LOG(MuthMModManager, Warning, TEXT("Unable to load Mod Info(%s) as Json data!"), *ModsFilename[i]);
			continue;
		}
		FModInfo CurModInfo;
		CurModInfo.ModID = ModInfoJson->GetIntegerField("ModID");
		CurModInfo.ModName = ModInfoJson->GetStringField("ModName");
		CurModInfo.MountPoint = ModInfoJson->GetStringField("MountPoint");
		CurModInfo.ModFileName = ModInfoJson->GetStringField("ModFileName");
		CurModInfo.ModInfoFile = ModsFilename[i];
		CurModInfo.ModSize = IFileManager::Get().FileSize(*FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("Mods"), CurModInfo.ModFileName));
		CurModInfo.bIsEnabled = (SaveGame->EnabledModID.Find(CurModInfo.ModID) != INDEX_NONE);
		//Maybe Localization in future.
		CurModInfo.ModDescription = FText::FromString(ModInfoJson->GetStringField("ModDescription"));
		ModsList.Add(CurModInfo);
	}
}

bool UModManagerImpl::LoadModInternal(FModInfo& ModInfo)
{
	if (!FCoreDelegates::OnMountPak.IsBound())
		return false;
	if (IsModLoaded(ModInfo.ModID))
	{
		UE_LOG(MuthMModManager, Error, TEXT("Mod(%s) has already been loaded!"), *ModInfo.ModName);
		return false;
	}
	if (ModInfo.MountPoint == "")
		return false;
	FString ModFullName = FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("Mods"), ModInfo.ModFileName);
	FString ModDir = FPaths::Combine(FPaths::ProjectModsDir(), ModInfo.MountPoint);
	FPaths::MakeStandardFilename(ModDir);
	if (!FCoreDelegates::OnMountPak.Execute(ModFullName,0,nullptr))
		return false;
	FPackageName::RegisterMountPoint(FString::Printf(TEXT("/%s/"), *ModInfo.MountPoint),ModDir);
	FString ModInstanceStr = FString::Printf(TEXT("/%s/ModInstance"), *ModInfo.MountPoint);
	UClass* ModInstanceClass = LoadClass<UModInstanceBase>(nullptr, *ModInstanceStr);
	if (ModInstanceClass)
	{
		auto* ModInstanceObj = NewObject<UModInstanceBase>(this, ModInstanceClass);
		if (ModInstanceObj)
		{
			ModInstanceObj->SetModID(ModInfo.ModID);
			ModInstanceObj->OnModInit();
			return true;
		}
	}
	return false;
}

void UModManagerImpl::InitLoadMods()
{
	if (_ModPlatformFile)
		return;
	IPlatformFile& InnerPlatform = FPlatformFileManager::Get().GetPlatformFile();
	_ModPlatformFile = new FPakPlatformFile();
	_ModPlatformFile->Initialize(&InnerPlatform, TEXT(""));
	_ModPlatformFile->InitializeNewAsyncIO();
	FPlatformFileManager::Get().SetPlatformFile(*_ModPlatformFile);
	RefreshModsList();
	for (auto& ModInfo : ModsList)
	{
		if (ModInfo.bIsEnabled)
		{
			if (!LoadModInternal(ModInfo))
			{
				UE_LOG(MuthMModManager, Warning, TEXT("Failed to load Mod:%s"), *ModInfo.ModName);
			}
		}
	}
}

TArray<FModInfo> UModManagerImpl::GetModsList() const
{
	return ModsList;
}

bool UModManagerImpl::LoadMod(int ModToLoad)
{
	FModInfo ModInfo;
	if (!_ModPlatformFile||!GetModInfoByID(ModToLoad, ModInfo))
		return false;
	return LoadModInternal(ModInfo);
}

void UModManagerImpl::UnloadMod(int ModToUnload)
{
	for (UModInstanceBase*& ModInstance : ModInstanceList)
	{
		if (ModInstance->GetModID() == ModToUnload)
		{
			ModInstance->OnModUnload();
			//TODO: Unload Mod
			return;
		}
	}
}

bool UModManagerImpl::IsModLoaded(int ModID) const
{
	for (UModInstanceBase*const& ModInstance:ModInstanceList)
	{
		if (ModInstance->GetModID() == ModID)
			return true;
	}
	return false;
}

void UModManagerImpl::SetModEnabled(int ModID, bool Enabled)
{
	for (auto& ModInfo:ModsList)
	{
		if (ModInfo.ModID==ModID)
		{
			ModInfo.bIsEnabled = Enabled;
			//Save to Save Game.
			auto SaveGame = Cast<UMuthMGameInstance>(GetWorld()->GetGameInstance())->GetGlobalSaveGame();
			if (Enabled)
			{
				SaveGame->EnabledModID.RemoveSingle(ModInfo.ModID);
			}
			else
			{
				SaveGame->EnabledModID.Add(ModInfo.ModID);
			}
			Cast<UMuthMGameInstance>(GetWorld()->GetGameInstance())->SaveGlobalSaveGame();
			return;
		}
	}
}

bool UModManagerImpl::GetModInfoByID(int ModID,FModInfo& Result) const
{
	for (auto& ModInfo:ModsList)
	{
		if (ModInfo.ModID == ModID)
		{
			Result = ModInfo;
			return true;
		}
	}
	return false;
}
