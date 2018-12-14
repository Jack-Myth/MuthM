// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "UserManager.h"
#include "UserManagerImpl.h"
#include "MuthMGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Add default functionality here for any IUserManager functions that are not pure virtual.

TScriptInterface<IUserManager> IUserManager::Get(const UObject* WorldContextObj)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObj));
	if (!GameInstance->UserManager.GetObject())
		GameInstance->UserManager= NewObject<UUserManagerImpl>(GameInstance);
	return GameInstance->UserManager;
}
