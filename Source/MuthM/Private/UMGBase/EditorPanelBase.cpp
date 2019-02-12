// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "EditorPanelBase.h"
#include "InEditorMode.h"
#include "Kismet/GameplayStatics.h"
#include "InstructionWidgetBase.h"
#include "InstructionManager.h"
#include "InGameMode.h"
#include "DetailsListBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "MainSoundWave.h"

#define LOCTEXT_NAMESPACE "MuthM"

void UEditorPanelBase::Init()
{
	//4 minutes music will be 24000x128,UINT A8,use 3MB mem.
	auto* EditorMode = Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this));
	_SpectrumTexture = EditorMode->DrawMainMusicSpectrum(0, GetMusicLength(), GetMusicLength() * 100, 128);
	OnSpectrumUpdate(_SpectrumTexture);
	EditorMode->OnMusicPlaybackTimeUpdate.AddDynamic(this, &UEditorPanelBase::OnMusicProcessCallback);

	//Load Instructions.
	auto EditorMMS = EditorMode->GetEditorMMS();
	TArray<UInstruction*> InstructionList = EditorMMS->GetAllInstructions();
	//Gen Instruction Widgets;
	for (int i = 0; i < InstructionList.Num(); i++)
	{
		auto* InstructionWidget = InstructionList[i]->GenInstructionWidget();
		InstructionWidget->Init(InstructionList[i]);
		InstructionWidgets.Add(InstructionWidget);
		OnInstructionWidgetAdded(InstructionWidget);
		InstructionWidget->InvalidateLayoutAndVolatility();
	}
}

void UEditorPanelBase::FillBPMInfo(float BPM)
{
	_BPM = BPM;
	OnFillBPMInfo(_BPM);
}

TArray<UInstructionWidgetBase*> UEditorPanelBase::GetOrderedInstructionWidgets()
{
	Algo::Sort(InstructionWidgets, [](const UInstructionWidgetBase* a, const UInstructionWidgetBase* b) {return a->GetInstructionTime() < b->GetInstructionTime(); });
	return InstructionWidgets;
}

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
		if (bIsInstructionInteractable)
		{
			_SelectedWidget = newClickedWidget;
			GetSelectedWidget()->OnWidgetSelected();
		}
	}
}

void UEditorPanelBase::OnClickHandler(float Time,float VerticalOffset)
{
	if (bFastAddMode&&bIsInstructionInteractable)
	{
		AddInstructionAtTime(TimeAxis,bIgnoreVerticalPosition?0.f:VerticalOffset);
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
			if (bIsInstructionInteractable)
			{
				AddInstructionAtTime(Time, bIgnoreVerticalPosition ? 0.f : VerticalOffset);
			}
		}
	}
}

void UEditorPanelBase::OnEditorPropertyInput(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
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
	if (PropertyName == "Time")
	{
		SetInstructionTime(TargetInstructionWidget, NumberValue);
	}
	else if (PropertyName == "__VisualVerticalOffset")
	{
		InstructionInstance->EditorVisualVerticalOffset = NumberValue;
		OnVerticalOffsetUpdate(TargetInstructionWidget, NumberValue);
	}
}

void UEditorPanelBase::AddInstructionAtTime(float Time,float VerticalOffset)
{
	if (!::IsValid(InstructionTemplate))
		return;
	auto* InEditorMode = Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this));
	check(InEditorMode);
	FBlueprintJsonObject BpJsonObj;
	BpJsonObj = InstructionTemplate->GenArgsJsonObject();
	if (bShouldAlignBPM)
	{
		float Offset = 60.f / _BPM * _AlignOffset;
		float a = Time / (60.f / (_BPM * _BeatDenominator));
		a = FMath::RoundToFloat(a);
		Time = a * (60.f / (_BPM * _BeatDenominator)) + Offset;
	}
	auto* InstructionInstance = IInstructionManager::Get(this)->GenInstruction(InstructionTemplate->GetRegisterName(), Time);
	if (!InstructionInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Add Instruction Failed!\nTemplate:%s,Register Name:%s"),*InstructionTemplate->GetName(),*InstructionTemplate->GetRegisterName().ToString());
		return;
	}
	FEditorExtraInfo EEI;
	EEI.ExtraInfoValid = true;
	EEI.VerticalOffset = VerticalOffset;
	InEditorMode->GetEditorMMS()->AddInstruction(InstructionInstance);
	InstructionInstance->OnInstructionLoaded_Editor(BpJsonObj,EEI);
	auto* InstructionWidget = InstructionInstance->GenInstructionWidget();
	InstructionWidget->Init(InstructionInstance);
	InstructionWidgets.Add(InstructionWidget);
	OnInstructionWidgetAdded(InstructionWidget);
	InstructionWidget->InvalidateLayoutAndVolatility();
}

void UEditorPanelBase::OnDetailListClosed(class UDetailsListBase* DetailsListWidget)
{
	_SelectedWidget = nullptr;
	ActivedDetailsWidget = nullptr;
}

void UEditorPanelBase::RemoveInstruction(class UInstructionWidgetBase* WidgetToRemove)
{
	if (InstructionWidgets.Remove(WidgetToRemove))
	{
		WidgetToRemove->RemoveFromParent();
		_SelectedWidget = nullptr;
		if (ActivedDetailsWidget)
		{
			ActivedDetailsWidget->CloseDetails();
			ActivedDetailsWidget = nullptr;
		}
		auto* InEditorMode = Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this));
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
	return Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this))->GetGameMainMusic()->GetSoundDuration();
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
	//Handle Time Input
	auto* TimeDetailItem = pInstructionCategory->ItemList.FindByPredicate([=](const TSharedPtr<FDetailItem>& a) {return a->Name == "Time"; });
	((FDetailItemNumber*)TimeDetailItem->Get())->DetailCallbackNumber.BindUFunction(this, "OnEditorPropertyInput");
	if (bShouldAlignBPM)
	{
		((FDetailItemNumber*)TimeDetailItem->Get())->SlideUnit = 60.f / _BPM / _BeatDenominator;
	}

	//Add Editor Property.
	FDetailCategoryStruct EditorDetails;
	TSharedPtr<FDetailItemNumber> VisualVerticalOffset=MakeShareable(new FDetailItemNumber());
	EditorDetails.Title = "Editor";
	EditorDetails.DisplayTitle = LOCTEXT("Editor", "Editor");
	VisualVerticalOffset->Name = "__VisualVerticalOffset";
	VisualVerticalOffset->DisplayName = LOCTEXT("VisualVerticalOffset", "Vertical Offset");
	VisualVerticalOffset->SlideMax = 1.f;
	VisualVerticalOffset->SlideMin = -1.f;
	VisualVerticalOffset->NumberValue = InstructionWidgetBase->GetInstructionInstance()->EditorVisualVerticalOffset;
	VisualVerticalOffset->InstructionInstance = InstructionWidgetBase->GetInstructionInstance();
	VisualVerticalOffset->DetailCallbackNumber.BindUFunction(this, "OnEditorPropertyInput");
	EditorDetails.ItemList.Add(VisualVerticalOffset);
	DetailsBuilder->AddCategory(EditorDetails);
	ActivedDetailsWidget = DetailsBuilder->GenDetailsWidget();
	ActivedDetailsWidget->OnDetailsListClosed.AddUObject(this, &UEditorPanelBase::OnDetailListClosed);
	ActivedDetailsWidget->AddToViewport(102);
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
	auto* tmpInstructionTemplate = IInstructionManager::Get(this)->GenInstruction(TemplateInstructionName,0);
	if (tmpInstructionTemplate)
	{
		//Replace current template and pupop details.
		InstructionTemplate = tmpInstructionTemplate;
		PupopTemplateDetails();
	}
}

bool UEditorPanelBase::ImportChart(const FString& FileName,TScriptInterface<class IChartImporter> Importer)
{
	auto* InEditorMode = Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this));
	if (!InEditorMode->GetEditorMMS()->Import(FileName, Importer))
		return false;
	TArray<UInstruction*> Instructions;
	Instructions= InEditorMode->GetEditorMMS()->GetAllInstructions();
	TArray<UInstructionWidgetBase*> InstructionWidgets = GetInstructionWidgets();
	for (int i=0;i<InstructionWidgets.Num();i++)
	{
		int Index = Instructions.Find(InstructionWidgets[i]->GetInstructionInstance());
		if (Index!=INDEX_NONE)
		{
			//Ignore the instruction which already have a widget.
			Instructions[Index] = nullptr;
		}
	}

	//Gen Instruction Widget
	for (int i=0;i<Instructions.Num();i++)
	{
		if (!Instructions[i])
			continue;
		auto* InstructionWidget = Instructions[i]->GenInstructionWidget();
		InstructionWidget->Init(Instructions[i]);
		InstructionWidgets.Add(InstructionWidget);
		OnInstructionWidgetAdded(InstructionWidget);
		InstructionWidget->InvalidateLayoutAndVolatility();
	}

	return true;
}

void UEditorPanelBase::SetEnableInstructionInteraction(bool bEnable)
{
	bIsInstructionInteractable = bEnable;
	_SelectedWidget = nullptr;
	if (ActivedDetailsWidget)
		ActivedDetailsWidget->CloseDetails();
}

TArray<class UInstructionWidgetBase*> UEditorPanelBase::CopyInstruction(TArray<class UInstructionWidgetBase*> InstructionsToCopy)
{
	TArray<UInstructionWidgetBase*> CopiedInstructions;
	auto* InEditorMode = Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this));
	for (int i=0;i<InstructionsToCopy.Num();i++)
	{
		UInstruction* TargetInstruction = InstructionsToCopy[i]->GetInstructionInstance();
		FBlueprintJsonObject BPJson = TargetInstruction->GenArgsJsonObject();
		auto InstructionMgr = IInstructionManager::Get(this);
		UInstruction* CopiedInstruction = InstructionMgr->GenInstruction(InstructionMgr->GetInstructionName(TargetInstruction->GetClass()), TargetInstruction->GetTime());
		InEditorMode->GetEditorMMS()->AddInstruction(CopiedInstruction);
		CopiedInstruction->OnInstructionLoaded_Editor(BPJson, FEditorExtraInfo());
		auto* InstructionWidget = CopiedInstruction->GenInstructionWidget();
		InstructionWidget->Init(CopiedInstruction);
		InstructionWidgets.Add(InstructionWidget);
		OnInstructionWidgetAdded(InstructionWidget);
		CopiedInstructions.Add(InstructionWidget);
	}
	return CopiedInstructions;
}

void UEditorPanelBase::SetInstructionTime(class UInstructionWidgetBase* InstructionWidget,float newTime)
{
	float LastTime = InstructionWidget->GetInstructionInstance()->GetTime();
	InstructionWidget->GetInstructionInstance()->SetTime(newTime);
	OnInstructionWidgetTimeChanged(InstructionWidget, LastTime, newTime);
}

#undef LOCTEXT_NAMESPACE