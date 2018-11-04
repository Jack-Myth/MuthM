// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionManagerImpl.h"

FInstructionRef UInstructionManagerImpl::RegisterInstruction(const FName& InstructionName, TSubclassOf<UInstruction> InstructionClass) const
{
	//TODO: Register Instruction
	return FInstructionRef();
}

bool UInstructionManagerImpl::UnregisterInstruction(const FInstructionRef InstructionRef) const
{ 
	//TODO: Unregister Instruction
	return false;
}
