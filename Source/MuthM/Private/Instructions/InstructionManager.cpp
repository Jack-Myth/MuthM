// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionManager.h"
#include "InstructionManagerImpl.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMInEditorMode.h"


TScriptInterface<IInstructionManager> IInstructionManager::Get()
{
	return GetMutableDefault<UInstructionManagerImpl>();
}

TScriptInterface<IMMScript> IInstructionManager::K2_GenMMScript()
{
	AGameModeBase* CurGameMode = UGameplayStatics::GetGameMode(GetWorldProvider());
	return GenMMScript(CurGameMode->GetClass()->IsChildOf<AMuthMInEditorMode>());
}

