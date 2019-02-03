// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "GlobalVariable.h"
#include "UIProvider.h"
#include "DetailInputSelectionBase.h"
#include "JsonObject.h"

#define LOCTEXT_NAMESPACE "MuthM"

void UGlobalVariable::OnSelectionInput(class UInstruction* InstructionInstance, FName PropertyName, class UDetailInputCustomBase* CustomWidget)
{
	auto* SelectionWidget = Cast<UDetailInputSelectionBase>(CustomWidget);
	if (PropertyName=="VariableType")
	{
		Type = SelectionWidget->GetCurrentSelection();
	}
}

void UGlobalVariable::OnStringInput(class UInstruction* InstructionInstance, FName PropertyName, FString StrValue)
{
	if (PropertyName=="StrValue")
	{
		this->StrValue = StrValue;
	}
	else if (PropertyName == "Key")
	{
		Key = StrValue;
	}
}

void UGlobalVariable::OnNumberInput(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	if (PropertyName=="NumberValue")
	{
		Number = NumberValue;
	}
}

void UGlobalVariable::InitProperties(FBlueprintJsonObject Args)
{
	Type = Args.Object->GetStringField("Type");
	Key = Args.Object->GetStringField("Key");
	if (Type=="String")
	{
		StrValue = Args.Object->GetStringField("StrValue");
	}
	else if (Type == "Number")
	{
		Number = Args.Object->GetNumberField("Number");
	}
	else
	{
		//This Instruction have nothing to do,destroy directly.
		DestroySelf(); 
	}
}

UGlobalVariable::UGlobalVariable()
{
	bHasPriority = true;
}

void UGlobalVariable::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	Super::OnInstructionLoaded_Implementation(Args);
	InitProperties(Args);
}

void UGlobalVariable::OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EditorExtraInfo)
{
	Super::OnInstructionLoaded_Editor_Implementation(Args,EditorExtraInfo);
	InitProperties(Args);
}

void UGlobalVariable::OnBuildingDetails_Implementation(TScriptInterface<IDetailsBuilder>& DetailsBuilder)
{
	Super::OnBuildingDetails_Implementation(DetailsBuilder);

	FDetailCategoryStruct GlobalVariable;
	GlobalVariable.Title = "GlobalVariable";
	GlobalVariable.DisplayTitle = LOCTEXT("GlobalVariable", "GlobalVariable");

	//Variable Type
	TSharedPtr<FDetailItemCustom> VariableType= MakeShareable(new FDetailItemCustom());
	VariableType->Name = "VariableType";
	VariableType->DisplayName = LOCTEXT("VariableType", "VariableType");
	VariableType->InstructionInstance = this;
	VariableType->DetailCallbackCustom.BindUFunction(this, "OnSelectionInput");
	auto* SelectionWidget = Cast<UDetailInputSelectionBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get(this)->GetDetailInputSelection(), NAME_None));
	TArray<FString> Options;
	Options.Add("String");
	Options.Add("Number");
	SelectionWidget->FillOptions(Options);
	SelectionWidget->SetCurrentSelection(Type);
	VariableType->CustomWidget = SelectionWidget;
	GlobalVariable.ItemList.Add(VariableType);


	TSharedPtr<FDetailItem> _VariableValue;
	if (Type=="String")
	{
		TSharedPtr<FDetailItemString> VariableValue = MakeShareable(new FDetailItemString());
		VariableValue->Name = "StrValue";
		VariableValue->DisplayName = LOCTEXT("StrValue", "StrValue");
		VariableValue->InstructionInstance = this;
		VariableValue->DetailCallbackStr.BindUFunction(this, "OnStringInput");
		VariableValue->StrValue = StrValue;
		_VariableValue = VariableValue;
	}
	else if (Type == "Number")
	{
		TSharedPtr<FDetailItemNumber> VariableValue = MakeShareable(new FDetailItemNumber());
		VariableValue->Name = "NumberValue";
		VariableValue->DisplayName = LOCTEXT("NumberValue", "NumberValue");
		VariableValue->InstructionInstance = this;
		VariableValue->DetailCallbackNumber.BindUFunction(this, "OnNumberInput");
		VariableValue->NumberValue = Number;
		_VariableValue = VariableValue;
	}
	if (_VariableValue.IsValid())
	{
		//Key
		TSharedPtr<FDetailItemString> VariableKey = MakeShareable(new FDetailItemString());
		VariableKey->Name = "Key";
		VariableKey->DisplayName = LOCTEXT("Key", "Key");
		VariableKey->InstructionInstance = this;
		VariableKey->DetailCallbackStr.BindUFunction(this, "OnStringInput");
		VariableKey->StrValue = Key;

		GlobalVariable.ItemList.Add(VariableKey);
		GlobalVariable.ItemList.Add(_VariableValue);
	}

	DetailsBuilder->AddCategory(GlobalVariable);
}

FBlueprintJsonObject UGlobalVariable::GenArgsJsonObject_Implementation()
{
	FBlueprintJsonObject BPJsonObj = Super::GenArgsJsonObject_Implementation();
	
	//Type
	BPJsonObj.Object->SetStringField("Type", Type);
	BPJsonObj.Object->SetStringField("Key", Key);
	if (Type=="String")
	{
		BPJsonObj.Object->SetStringField("StrValue", StrValue);
	}
	else if (Type == "Number")
	{
		BPJsonObj.Object->SetNumberField("Number", Number);
	}
	return BPJsonObj;
}

void UGlobalVariable::OnPrepare_Implementation()
{
	if (Type=="String")
	{
		SetGlobalStringData(*Key, StrValue);
	}
	else if (Type == "Number")
	{
		SetGlobalNumberData(*Key, Number);
	}
	DestroySelf();
}

#undef LOCTEXT_NAMESPACE