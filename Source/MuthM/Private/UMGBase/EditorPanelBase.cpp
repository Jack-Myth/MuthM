// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "EditorPanelBase.h"
#include "MuthMInEditorMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "InstructionWidgetBase.h"
#include "InstructionManager.h"

void UEditorPanelBase::SetSpectrumRes(int ResX, int ResY)
{
	_SpectrumRenderTarget->ResizeTarget(ResX, ResY);
}

void UEditorPanelBase::RefreshSpectrum()
{
	auto* EditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(EditorMode);
	EditorMode->DrawMainMusicSpectrum(_SpectrumRenderTarget,PanelLBorder,PanelRBorder,_SpectrumRenderTarget->SizeX,_SpectrumRenderTarget->SizeY);
	OnSpectrumUpdate(_SpectrumRenderTarget);
}

void UEditorPanelBase::SetTimelineScale(float NewScaleRatio)
{
	constexpr float StdTimeLength = 30;
	float BorderDistance = StdTimeLength * NewScaleRatio*0.5;
	float ScaleAxis = FMath::Lerp(PanelLBorder, PanelRBorder, 0.5);
	PanelLBorder = ScaleAxis - BorderDistance;
	PanelRBorder = ScaleAxis + BorderDistance;
	//Check if PanelL is less than 0
	if (PanelLBorder<0)
	{
		PanelRBorder -= PanelLBorder;//(+=-PanelLBorder)
		PanelLBorder = 0;
	}
	OnPanelBoundChanged(PanelLBorder, PanelRBorder);
	RefreshSpectrum();
}

void UEditorPanelBase::SetTimeAxis(float NewTime)
{
	TimeAxis = NewTime;
	OnTimeAxisChanged(NewTime);
}

void UEditorPanelBase::ClickWidget(class UInstructionWidgetBase* newClickedWidget)
{
	if (newClickedWidget == _SelectedWidget)
		return;
	else
	{
		GetSelectedWidget()->OnWidgetDeselected();
		_SelectedWidget = newClickedWidget;
		GetSelectedWidget()->OnWidgetSelected();
	}
}

void UEditorPanelBase::OnClickHandler(float Time)
{
	if (bFastAddMode)
	{
		auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
		check(InEditorMode);
		auto* InstructionInstance = IInstructionManager::Get()->GenInstruction(_FastAddInstructionName, Time, FJsonObject());
		auto* InstructionWidget = InstructionInstance->GenInstructionWidget();
		InstructionWidget->Init(InstructionInstance);
		InstructionWidgets.Add(InstructionWidget);
	}
	else
	{
		if (_SelectedWidget)
		{
			//Deselect Widget;
			//First Notify Widget
			_SelectedWidget->OnWidgetDeselected();
			//Second Notify EditorPanel.
			OnInstructionDeselected(_SelectedWidget);
			_SelectedWidget = nullptr;
		}
	}
}

void UEditorPanelBase::RemoveInstruction(class UInstructionWidgetBase* WidgetToRemove)
{
	if (InstructionWidgets.Remove(WidgetToRemove))
	{
		WidgetToRemove->RemoveFromParent();
		auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
		InEditorMode->GetEditorMMS()->RemoveInstruction(WidgetToRemove->GetInstructionInstance(), EInstructionDestroyReason::IDR_Editing);
	}
}

void UEditorPanelBase::NativeConstruct()
{
	_SpectrumRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this,256,64);
}
