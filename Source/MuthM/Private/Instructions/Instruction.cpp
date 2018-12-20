// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "Instruction.h"
#include "MMScript.h"
#include "UIProvider.h"
#include "InstructionWidgetBase.h"

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

class UInstructionWidgetBase* UInstruction::GenInstructionWidget_Implementation()
{
	auto InstructionWidgetClass = UUIProvider::Get(this)->GetInstructionWidget();
	return Cast<UInstructionWidgetBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), InstructionWidgetClass, NAME_None));
}

void UInstruction::SetWorld(UWorld* WorldContext)
{
	CachedWorld = MakeShareable(WorldContext);
}