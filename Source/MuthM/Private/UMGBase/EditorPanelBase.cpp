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
		GetSelectedWidget()->OnWidgetSelected();
	else
	{
		if (GetSelectedWidget())
			GetSelectedWidget()->OnWidgetDeselected();
		if (ActivedDetailsWidget)
			ActivedDetailsWidget->CloseDetails();
		_SelectedWidget = newClickedWidget;
		GetSelectedWidget()->OnWidgetSelected();
	}
}

void UEditorPanelBase::OnClickHandler(float Time,float VerticleOffset)
{
	if (bFastAddMode)
	{
		AddInstructionAtTime(TimeAxis);
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
			if (ActivedDetailsWidget)
				ActivedDetailsWidget->CloseDetails();
		}
		if (_NextToAdd)
		{
			_NextToAdd = false;
			AddInstructionAtTime(Time);
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

void UEditorPanelBase::AddInstructionAtTime(float Time)
{
	if (!::IsValid(InstructionTemplate))
		return;
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(InEditorMode);
	FBlueprintJsonObject BpJsonObj;
	BpJsonObj = InstructionTemplate->GenArgsJsonObject();
	auto* InstructionInstance = IInstructionManager::Get(this)->GenInstruction(InstructionTemplate->GetRegisterName(), Time, *BpJsonObj.Object);
	if (!InstructionInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Add Instruction Failed!\nTemplate:%s,Register Name:%s"),*InstructionTemplate->GetName(),*InstructionTemplate->GetRegisterName().ToString());
		return;
	}
	InEditorMode->GetEditorMMS()->AddInstruction(InstructionInstance);
	auto* InstructionWidget = InstructionInstance->GenInstructionWidget();
	InstructionWidget->Init(InstructionInstance);
	InstructionWidgets.Add(InstructionWidget);
	OnInstructionWidgetAdded(InstructionWidget);
}

void UEditorPanelBase::OnDetailListClosed(class UDetailsListBase* DetailsListWidget)
{
	_SelectedWidget = nullptr;
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
		ActivedDetailsWidget->CloseDetails();
	}
}

float UEditorPanelBase::GetMusicLength()
{
	return Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this))->GetGameMainMusic()->GetSoundDuration();
}

void UEditorPanelBase::PupopDetails(class UInstructionWidgetBase* InstructionWidgetBase)
{
	auto DetailsBuilder = IDetailsBuilder::GenNewBuilder();
	DetailsBuilder->SetDetailsHolder(InstructionWidgetBase->GetInstructionInstance());
	DetailsBuilder->CollectDetails();
	auto* pInstructionCategory = DetailsBuilder->FindCategoryByName("Instruction");
	if (!pInstructionCategory)
		return;
	if (ActivedDetailsWidget)
	{
		ActivedDetailsWidget->CloseDetails();
		ActivedDetailsWidget = nullptr;
	}
	_SelectedWidget = InstructionWidgetBase;
	auto* TimeDetailItem = pInstructionCategory->ItemList.FindByPredicate([=](const TSharedPtr<FDetailItem>& a) {return a->Name == "Time"; });
	((FDetailItemNumber*)TimeDetailItem->Get())->DetailCallbackNumber.BindUFunction(this, "OnInstructionTimeInput");
	if (bShouldAlignBPM)
	{
		((FDetailItemNumber*)TimeDetailItem->Get())->SlideUnit = 60.f / _BPM / BeatDenominator;
	}
	ActivedDetailsWidget = DetailsBuilder->GenDetailsWidget();
	ActivedDetailsWidget->OnDetailsListClosed.AddUObject(this, &UEditorPanelBase::OnDetailListClosed);
	ActivedDetailsWidget->AddToViewport(100);
}

void UEditorPanelBase::PupopTemplateDetails()
{
	_SelectedWidget = nullptr;
	auto DetailsBuilder = IDetailsBuilder::GenNewBuilder();
	DetailsBuilder->SetDetailsHolder(InstructionTemplate);
	DetailsBuilder->CollectDetails();
	auto* pInstructionCategory = DetailsBuilder->FindCategoryByName("Instruction");
	if (pInstructionCategory)
	{
		pInstructionCategory->ItemList.RemoveAll([=](const TSharedPtr<FDetailItem>& a) {return a->Name == "Time"; });
	}
	ActivedDetailsWidget = DetailsBuilder->GenDetailsWidget();
	ActivedDetailsWidget->AddToViewport(100);
}

void UEditorPanelBase::OnMusicProcessCallback(float Current, float Duration)
{
	SetTimeAxis(Current);
}

void UEditorPanelBase::SetInstructionTemplateByName(FName TemplateInstructionName)
{
	auto* tmpInstructionTemplate = IInstructionManager::Get(this)->GenInstruction(TemplateInstructionName,0, FJsonObject());
	if (tmpInstructionTemplate)
	{
		//Replace current template and pupop details.
		InstructionTemplate = tmpInstructionTemplate;
		PupopTemplateDetails();
	}
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