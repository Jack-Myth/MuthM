// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "EditorPanelBase.h"
#include "MuthMInEditorMode.h"
#include "Kismet/GameplayStatics.h"
#include "InstructionWidgetBase.h"
#include "InstructionManager.h"
#include "MuthMInGameMode.h"
#include "DetailsListBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "MainSoundWave.h"

#define LOCTEXT_NAMESPACE "MuthM"

void UEditorPanelBase::SetTimeAxis(float NewTime)
{
	if (NewTime == TimeAxis)
		return;
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
		auto* InstructionInstance = IInstructionManager::Get(this)->GenInstruction(_FastAddInstructionName, TimeAxis, EmptyJsonObj);
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
		TargetInstructionWidget = _SelectedWidget;
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

void UEditorPanelBase::DeleteCurrentInstruction()
{
	if (_SelectedWidget)
	{
		RemoveInstruction(_SelectedWidget);
	}
}

float UEditorPanelBase::GetMusicLength()
{
	return Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this))->GetGameMainMusic()->GetSoundDuration();
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
	if (bShouldAlignBPM)
	{
		auto TimeDetailItem = InstructionCategory.ItemList.FindByPredicate([=](const TSharedPtr<FDetailItem>& a) {return a->Name == "Time"; });
		((FDetailItemNumber*)TimeDetailItem->Get())->SlideUnit = 60.f / _BPM / BeatDenominator;
	}
	ActivedDetailsWidget = DetailsBuilder->GenDetails();
	ActivedDetailsWidget->AddToViewport(100);
}

void UEditorPanelBase::OnMusicProcessCallback(float Current, float Duration)
{
	SetTimeAxis(Current);
}

void UEditorPanelBase::NativeConstruct()
{
	Super::NativeConstruct();
	//4 minutes music will be 24000x128,UINT A8,use 3MB mem.
	auto* EditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	_SpectrumTexture = EditorMode->DrawMainMusicSpectrum(0,GetMusicLength(),GetMusicLength() * 100, 128);
	OnSpectrumUpdate(_SpectrumTexture);
	EditorMode->OnMusicPlaybackTimeUpdate.AddDynamic(this, &UEditorPanelBase::OnMusicProcessCallback);
}

#undef LOCTEXT_NAMESPACE