// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicManager.h"
#include "MuthMGameInstance.h"
#include "MusicManagerImpl.h"
#include "Kismet/GameplayStatics.h"

TScriptInterface<IMusicManager> IMusicManager::Get(const UObject* WorldContextObj)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObj));
	if (!GameInstance->MusicManager.GetObject())
		GameInstance->MusicManager= NewObject<UMusicManagerImpl>(GameInstance);
	return GameInstance->MusicManager;
}
