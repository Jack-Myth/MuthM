// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionManager.h"
#include "InstructionManagerImpl.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameInstance.h"

TScriptInterface<IInstructionManager> IInstructionManager::Get(const UObject* WorldContextObj)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObj));
	if (!GameInstance->InstructionManager.GetObject())
		GameInstance->InstructionManager = NewObject<UInstructionManagerImpl>(GameInstance);
	return GameInstance->InstructionManager;
}

