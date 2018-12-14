// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ModManager.h"
#include "ModManagerImpl.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameInstance.h"

// Add default functionality here for any IModManager functions that are not pure virtual.

TScriptInterface<IModManager> IModManager::Get(const UObject* WorldContextObj)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObj));
	if (!GameInstance->ModManager.GetObject())
		GameInstance->ModManager= NewObject<UModManagerImpl>(GameInstance);
	return GameInstance->ModManager;
}
