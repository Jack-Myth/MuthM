// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "EditorPanelBase.h"
#include "MuthMInEditorMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "InstructionWidgetBase.h"
#include "InstructionManager.h"
#include "MuthMInGameMode.h"
#include "DetailsListBase.h"

#define LOCTEXT_NAMESPACE "MuthM"

void UEditorPanelBase::SetTimeAxis(float NewTime)
{
	TimeAxis = NewTime;
	OnTimeAxisChanged(NewTime);
	if (bFastAddMode)
		OnCenterTimeAxis();
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
		FJsonObject EmptyJsonObj;
		auto* InstructionInstance = IInstructionManager::Get()->GenInstruction(_FastAddInstructionName, TimeAxis, EmptyJsonObj);
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

void UEditorPanelBase::OnInstructionTimeInput(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	UInstructionWidgetBase* TargetInstructionWidget=nullptr;
	if (_SelectedWidget->GetInstructionInstance() == InstructionInstance)
		TargetInstructionWidget;
	else
	{
		for (int i = 0; i < InstructionWidgets.Num(); i++)
		{
			if (InstructionWidgets[i]->GetInstructionInstance()==InstructionInstance)
			{
				TargetInstructionWidget = InstructionWidgets[i];
				break;
			}
		}
	}
	check(TargetInstructionWidget);
	float LastTime = InstructionInstance->GetTime();
	InstructionInstance->SetTime(NumberValue);
	OnInstructionWidgetTimeChanged(TargetInstructionWidget, LastTime, NumberValue);
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

float UEditorPanelBase::GetMusicLength()
{
	return Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this))->GetGameMainMusic()->GetDuration();
}

void UEditorPanelBase::PupopDetails(class UInstructionWidgetBase* InstructionWidgetBase)
{
	if (ActivedDetailsWidget)
	{
		ActivedDetailsWidget->RemoveFromParent();
		ActivedDetailsWidget = nullptr;
	}
	auto DetailsBuilder = IDetailsBuilder::GenNewBuilder();
	DetailsBuilder->SetDetailsHolder(InstructionWidgetBase->GetInstructionInstance());
	FDetailCategoryStruct InstructionCategory;
	InstructionCategory.Title = "Instruction";
	InstructionCategory.DisplayTitle = LOCTEXT("Instruction", "Instruction");
	FDetailItemNumber* TimeInput = new FDetailItemNumber();
	TimeInput->Name = "Time";
	TimeInput->DisplayName = LOCTEXT("Time", "Time");
	TimeInput->NumberValue = InstructionWidgetBase->GetInstructionInstance()->GetTime();
	TimeInput->DetailCallbackNumber.BindUFunction(this, "OnInstructionTimeInput");
	TimeInput->InstructionInstance = InstructionWidgetBase->GetInstructionInstance();
	TSharedPtr <FDetailItem> pTimeInput = MakeShareable(TimeInput);
	InstructionCategory.ItemList.Add(pTimeInput);
	ActivedDetailsWidget = DetailsBuilder->GenDetails();
	ActivedDetailsWidget->AddToViewport(100);
}

void UEditorPanelBase::NativeConstruct()
{
	//4 minutes music will be 7200x64,UINT R8,use 0.4MB mem.
	_SpectrumRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, GetMusicLength() * 30*ScaleRatio, 64,ETextureRenderTargetFormat::RTF_R8);
	auto* EditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(EditorMode);
	EditorMode->DrawMainMusicSpectrum(_SpectrumRenderTarget, 0,GetMusicLength(), _SpectrumRenderTarget->SizeX, _SpectrumRenderTarget->SizeY);
	OnSpectrumUpdate(_SpectrumRenderTarget);
}

#undef LOCTEXT_NAMESPACE