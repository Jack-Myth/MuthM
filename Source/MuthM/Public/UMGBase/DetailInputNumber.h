// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailInputNumber.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UDetailInputNumber : public UUserWidget
{
	GENERATED_BODY()

	TSharedPtr<struct FDetailItemNumber> _DetailItemInfo;
public:
	UFUNCTION(BlueprintCallable)
		void FireOnValueChanged();
	void ValueInit(TSharedPtr<struct FDetailItemNumber> DetailItemInfo);
	UFUNCTION(BlueprintImplementableEvent)
		float GetInputNumber();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnValueInit"))
		void K2_OnValueInit(FText DisplayName, class UInstruction* InstructionInstance,float NumberValue);
};
