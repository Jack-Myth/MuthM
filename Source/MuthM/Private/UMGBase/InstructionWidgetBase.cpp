// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionWidgetBase.h"
#include "Instruction.h"
#include "DetailsBuilder.h"
#include "InEditorMode.h"
#include "Kismet/GameplayStatics.h"
#include "EditorMainUIBase.h"
#include "EditorPanelBase.h"

void UInstructionWidgetBase::Init(class UInstruction* InstructionInstance)
{
	_InstructionInstance = InstructionInstance;
}

float UInstructionWidgetBase::GetInstructionTime() const
{
	return GetInstructionInstance()->GetTime();
}

void UInstructionWidgetBase::ClickEventHandler()
{
	auto* InEditorMode = Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(InEditorMode);
	InEditorMode->GetEditorMainUI()->GetEditorPanel()->ClickWidget(this);
}

void UInstructionWidgetBase::DestroyInstruction()
{
	auto* InEditorMode = Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(InEditorMode);
	InEditorMode->GetEditorMainUI()->GetEditorPanel()->RemoveInstruction(this);
}

void UInstructionWidgetBase::SetVerticalOffset(float Offset)
{
	OnVerticalOffset(Offset);
}

void UInstructionWidgetBase::RefreshOffset()
{
	auto* InEditorMode = Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(InEditorMode);
	float HalfHeight = InEditorMode->GetEditorMainUI()->GetEditorPanel()->GetPanelHalfHeight();
	OnRefreshOffset(HalfHeight);
}

void UInstructionWidgetBase::OnWidgetSelected_Implementation()
{
	auto* InEditorMode = Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(InEditorMode);
	InEditorMode->GetEditorMainUI()->GetEditorPanel()->PupopDetails(this);
}
