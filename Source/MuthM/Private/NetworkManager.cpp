// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "NetworkManager.h"
#include "NetworkManagerImpl.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameInstance.h"

// Add default functionality here for any INetworkManager functions that are not pure virtual.

TScriptInterface<INetworkManager> INetworkManager::Get(const UObject* WorldContextObj)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObj));
	if (!GameInstance->NetworkManager.GetObject())
		GameInstance->NetworkManager= NewObject<UNetworkManagerImpl>(GameInstance);
	return GameInstance->NetworkManager;
}
