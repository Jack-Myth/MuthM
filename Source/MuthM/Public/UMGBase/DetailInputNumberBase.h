// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailInputNumberBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UDetailInputNumberBase : public UUserWidget
{
	GENERATED_BODY()

	TSharedPtr<struct FDetailItemNumber> _DetailItemInfo;
public:
	UFUNCTION(BlueprintCallable)
		void FireOnValueChanged();
	void ValueInit(TSharedPtr<struct FDetailItemNumber> DetailItemInfo);
	UFUNCTION(BlueprintImplementableEvent)
		float GetInputNumber();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Value Init"))
		void K2_OnValueInit(const FText& DisplayName, class UInstruction* InstructionInstance,float NumberValue);
};
