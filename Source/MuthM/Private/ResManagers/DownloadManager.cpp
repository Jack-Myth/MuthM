// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DownloadManager.h"
#include "DownloadManagerImpl.h"
#include "MuthMGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Add default functionality here for any IDownloadManager functions that are not pure virtual.

TScriptInterface<IDownloadManager> IDownloadManager::Get(const UObject* WorldContextObj)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObj));
	if (!GameInstance->DownloadManager.GetObject())
		GameInstance->DownloadManager= NewObject<UDownloadManagerImpl>(GameInstance);
	return GameInstance->DownloadManager;
}
