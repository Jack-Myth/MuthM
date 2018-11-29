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
		class UInstruction* _InstructionInstance;
public:
	void Init(class UInstruction* InstructionInstance);
	UFUNCTION(BlueprintPure)
		FORCEINLINE class UInstruction* GetInstructionInstance()
	{
		return _InstructionInstance;
	}
	UFUNCTION(BlueprintPure)
		float GetInstructionTime();
	UFUNCTION(BlueprintCallable)
		void ClickEventHandler();
	UFUNCTION(BlueprintImplementableEvent)
		void OnWidgetSelected();
	UFUNCTION(BlueprintImplementableEvent)
		void OnWidgetDeselected();
	UFUNCTION(BlueprintImplementableEvent)
		void OnTwikleWidget();
	UFUNCTION(BlueprintCallable)
		void PupopDetails();
};
