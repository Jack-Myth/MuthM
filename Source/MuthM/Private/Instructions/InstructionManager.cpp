// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionManager.h"
#include "InstructionManagerImpl.h"


// Add default functionality here for any IInstructionManager functions that are not pure virtual.


TScriptInterface<IInstructionManager> IInstructionManager::Get()
{
	return GetMutableDefault<UInstructionManagerImpl>();
}