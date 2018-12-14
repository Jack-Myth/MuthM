// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionManager.h"
#include "InstructionManagerImpl.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMInEditorMode.h"
#include "MuthMGameInstance.h"

TScriptInterface<IInstructionManager> IInstructionManager::Get(const UObject* WorldContextObj)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObj));
	if (!GameInstance->InstructionManager.GetObject())
		GameInstance->InstructionManager = NewObject<UInstructionManagerImpl>(GameInstance);
	return GameInstance->InstructionManager;
}

TScriptInterface<IMMScript> IInstructionManager::K2_GenMMScript()
{
	AGameModeBase* CurGameMode = UGameplayStatics::GetGameMode(GetWorldProvider());
	return GenMMScript(CurGameMode->GetClass()->IsChildOf<AMuthMInEditorMode>());
}

