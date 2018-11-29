// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionWidgetBase.h"
#include "Instruction.h"
#include "DetailsBuilder.h"
#include "MuthMInEditorMode.h"
#include "Kismet/GameplayStatics.h"
#include "EditorMainUIBase.h"
#include "EditorPanelBase.h"

void UInstructionWidgetBase::Init(class UInstruction* InstructionInstance)
{
	_InstructionInstance = InstructionInstance;
}

float UInstructionWidgetBase::GetInstructionTime()
{
	return GetInstructionInstance()->GetTime();
}

void UInstructionWidgetBase::ClickEventHandler()
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(InEditorMode);
	InEditorMode->GetEditorMainUI()->GetEditorPanel()->ClickWidget(this);
}

void UInstructionWidgetBase::PupopDetails()
{
	if (GetInstructionInstance()->GetClass()->ImplementsInterface(UHasDetails::StaticClass()))
	{
		auto DetailsBuilder = IDetailsBuilder::GenNewBuilder();
		DetailsBuilder->SetDetailsHolder(GetInstructionInstance());
		DetailsBuilder->GenDetails()->AddToViewport(100);
	}
}
