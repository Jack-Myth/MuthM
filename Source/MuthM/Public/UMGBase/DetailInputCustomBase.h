// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailInputCustomBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UDetailInputCustomBase : public UUserWidget
{
	GENERATED_BODY()
	
	TSharedPtr<struct FDetailItemCustom> _DetailItemInfo;
public:
	UFUNCTION(BlueprintCallable)
		void FireOnValueChanged();
	void ValueInit(TSharedPtr<struct FDetailItemCustom> DetailItemInfo);

	UFUNCTION(BlueprintImplementableEvent,meta=(DisplayName="OnValueInit"))
		void K2_OnValueInit(const FText& DisplayName, class UInstruction* InstructionInstance);
};
