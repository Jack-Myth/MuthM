// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailInputStringBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UDetailInputStringBase : public UUserWidget
{
	GENERATED_BODY()

	TSharedPtr<struct FDetailItemString> _DetailItemInfo;
public:
	UFUNCTION(BlueprintCallable)
		void FireOnValueChanged();
	void ValueInit(TSharedPtr<struct FDetailItemString> DetailItemInfo);
	UFUNCTION(BlueprintImplementableEvent)
		float GetInputNumber();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnValueInit"))
		void K2_OnValueInit(const FText& DisplayName, class UInstruction* InstructionInstance, const FString& StrValue);
};
