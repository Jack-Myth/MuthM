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

void UInstructionWidgetBase::DestroyInstruction()
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(InEditorMode);
	InEditorMode->GetEditorMainUI()->GetEditorPanel()->RemoveInstruction(this);
}

void UInstructionWidgetBase::OnWidgetSelected_Implementation()
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(InEditorMode);
	InEditorMode->GetEditorMainUI()->GetEditorPanel()->PupopDetails(this);
}
