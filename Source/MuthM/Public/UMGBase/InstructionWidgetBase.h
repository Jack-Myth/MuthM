// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InstructionWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UInstructionWidgetBase : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
		class UInstruction* InstructionInstance;
public:
	void Init(class UInstruction* InstructionInstance);
	UFUNCTION(BlueprintPure)
		float GetInstructionTime();
};
