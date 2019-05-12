// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "Instruction.h"
#include "MMScript.h"
#include "UIProvider.h"
#include "InstructionWidgetBase.h"
#include "InGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObject.h"
#include "DetailStructures.h"
#include "InstructionManager.h"

#define LOCTEXT_NAMESPACE "MuthM"

void UInstruction::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{}

void UInstruction::OnPrepare_Implementation()
{}

void UInstruction::OnTick_Implementation(float CurrentTime)
{}

void UInstruction::OnInstructionDestroy_Implementation(EInstructionDestroyReason DestroyReason)
{}

void UInstruction::OnTimeArrived_Implementation()
{}

FBlueprintJsonObject UInstruction::GenArgsJsonObject_Implementation()
{
	FBlueprintJsonObject bpJson;
	bpJson.Object = MakeShareable(new FJsonObject());
	return bpJson;
}

void UInstruction::OnTimeChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	SetTime(NumberValue);
	//Reorder MMS Data
	GetScript()->UpdateInstructions();
}

FName UInstruction::GetRegisterName() const
{
	return IInstructionManager::Get(this)->GetInstructionName(GetClass());
}

bool UInstruction::IsInstructionReady_Implementation(float CurrentTime) const
{
	return false;
}

void UInstruction::OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args,FEditorExtraInfo EditorExtraInfo)
{}

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
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	return InGameMode->GlobalDataNumber.FindRef(Key);
}

void UInstruction::SetGlobalNumberData(FName Key, float Value)
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	InGameMode->GlobalDataNumber.FindOrAdd(Key) = Value;
}

FString UInstruction::GetGlobalStringData(FName Key) const
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	return InGameMode->GlobalDataString.FindRef(Key);
}

void UInstruction::SetGlobalStringData(FName Key, FString String)
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	InGameMode->GlobalDataString.FindOrAdd(Key) = String;
}

UObject* UInstruction::GetGlobalObjectData(FName Key) const
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	return InGameMode->GlobalDataObject.FindRef(Key);
}

void UInstruction::SetGlobalObjectData(FName Key, UObject* Object)
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	InGameMode->GlobalDataObject.Add(Key) = Object;
}

class UWorld* UInstruction::GetWorld() const
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		UObject* Outer = GetOuter();
		while (Outer && !Outer->GetWorld())
			Outer = Outer->GetOuter();
		return Outer ? Outer->GetWorld() : nullptr;
	}
	return nullptr;
}

void UInstruction::OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder)
{
	//Super::OnBuildingDetails_Implementation(DetailsBuilder);
	//No need to call Super class

	//Add Time
	FDetailCategoryStruct DetailCategory;
	DetailCategory.Title = "Instruction";
	DetailCategory.DisplayTitle = LOCTEXT("Instruction", "Instruction");
	TSharedPtr<FDetailItemNumber> TimeDetail=MakeShareable(new FDetailItemNumber());
	TimeDetail->Name = "Time";
	TimeDetail->InstructionInstance = this;
	TimeDetail->NumberValue = GetTime();
	TimeDetail->DisplayName = LOCTEXT("Time", "Time");
	TimeDetail->DetailCallbackNumber.BindUFunction(this, "OnTimeChanged");
	DetailCategory.ItemList.Add(TimeDetail);
	DetailsBuilder->AddCategory(DetailCategory);
}

class UInstructionWidgetBase* UInstruction::GenInstructionWidget_Implementation()
{
	auto InstructionWidgetClass = UUIProvider::Get(this)->GetInstructionWidget();
	return Cast<UInstructionWidgetBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), InstructionWidgetClass, NAME_None));
}

#undef LOCTEXT_NAMESPACE