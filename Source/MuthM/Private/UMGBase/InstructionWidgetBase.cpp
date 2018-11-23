// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionWidgetBase.h"
#include "Instruction.h"

void UInstructionWidgetBase::Init(class UInstruction* InstructionInstance)
{

}

float UInstructionWidgetBase::GetInstructionTime()
{
	return InstructionInstance->GetTime();
}
