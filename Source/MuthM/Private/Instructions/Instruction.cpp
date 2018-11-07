// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "Instruction.h"
#include "MMScript.h"

float UInstruction::GetTime()
{
	return InstructionTime;
}

void UInstruction::SetTime(float pNewTime)
{
	InstructionTime = pNewTime;
}

void UInstruction::DestroySelf()
{
	if (AttachedScript)
	{
		AttachedScript->RemoveInstruction(this, EInstructionDestroyReason::IDR_Manually);
	}
	else
	{
		this->RemoveFromRoot();
		this->MarkPendingKill();
	}
}

void UInstruction::SetWorld(UWorld* WorldContext)
{
	CachedWorld = MakeShareable(WorldContext);
}