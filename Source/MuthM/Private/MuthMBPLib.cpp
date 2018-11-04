// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMBPLib.h"
#include "InstructionManagerImpl.h"

TScriptInterface<IInstructionManager> UMuthMBPLib::GetInstructionManager()
{
	return IInstructionManager::Get();
}
