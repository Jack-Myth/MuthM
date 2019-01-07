// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicManager.h"
#include "MuthMGameInstance.h"
#include "Kismet/GameplayStatics.h"

#ifdef _MUTHM_USE_FMOD
#include "MusicManagerImplFMod.h"
#define MUSICMANAGER_CLASS UMusicManagerImplFMod
#else
#include "MusicManagerImpl.h"
#define MUSICMANAGER_CLASS UMusicManagerImpl
#endif // _MUTHM_USE_FMOD


TScriptInterface<IMusicManager> IMusicManager::Get(const UObject* WorldContextObj)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObj));
	if (!GameInstance->MusicManager.GetObject())
		GameInstance->MusicManager= NewObject<MUSICMANAGER_CLASS>(GameInstance);
	return GameInstance->MusicManager;
}
