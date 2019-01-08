// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "Instruction.h"
#include "MMScript.h"
#include "UIProvider.h"
#include "InstructionWidgetBase.h"
#include "MuthMInGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObject.h"

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
	auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
	return InGameMode->GlobalDataNumber.FindRef(Key);
}

void UInstruction::SetGlobalNumberData(FName Key, float Value)
{
	auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
	InGameMode->GlobalDataNumber.FindOrAdd(Key) = Value;
}

class UInstructionWidgetBase* UInstruction::GenInstructionWidget_Implementation()
{
	auto InstructionWidgetClass = UUIProvider::Get(this)->GetInstructionWidget();
	return Cast<UInstructionWidgetBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), InstructionWidgetClass, NAME_None));
}