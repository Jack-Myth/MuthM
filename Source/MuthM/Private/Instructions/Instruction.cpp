// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "Instruction.h"

float UInstruction::GetTime()
{
	return InstructionTime;
}

void UInstruction::SetTime(float pNewTime)
{
	InstructionTime = pNewTime;
}

void UInstruction::SetWorld(UWorld* WorldContext)
{
	CachedWorld = MakeShareable(WorldContext);
}