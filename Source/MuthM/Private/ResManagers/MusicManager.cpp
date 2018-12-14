// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicManager.h"
#include "MusicManagerImpl.h"
#include "MuthMGameInstance.h"
#include "Kismet/GameplayStatics.h"


// Add default functionality here for any IMusicManager functions that are not pure virtual.

TScriptInterface<IMusicManager> IMusicManager::Get(const UObject* WorldContextObj)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObj));
	if (!GameInstance->MusicManager.GetObject())
		GameInstance->MusicManager= NewObject<UMusicManagerImpl>(GameInstance);
	return GameInstance->MusicManager;
}
