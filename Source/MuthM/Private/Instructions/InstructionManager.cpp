// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionManager.h"
#include "InstructionManagerImpl.h"


TScriptInterface<IInstructionManager> IInstructionManager::Get()
{
	return GetMutableDefault<UInstructionManagerImpl>();
}

