// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "Instruction.h"
#include "MMScript.h"
#include "UIProvider.h"
#include "InstructionWidgetBase.h"
#include "MuthMInGameMode.h"
#include "Kismet/GameplayStatics.h"

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

float UInstruction::GetGlobalNumberData(FName Key) const
{
	auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
	return InGameMode->GlobalDataNumber.FindRef(Key);
}

class UInstructionWidgetBase* UInstruction::GenInstructionWidget_Implementation()
{
	auto InstructionWidgetClass = UUIProvider::Get(this)->GetInstructionWidget();
	return Cast<UInstructionWidgetBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), InstructionWidgetClass, NAME_None));
}