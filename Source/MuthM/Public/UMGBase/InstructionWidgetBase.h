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
protected:
	UFUNCTION(BlueprintImplementableEvent)
		void OnVerticalOffset(float Offset);
	UFUNCTION(BlueprintImplementableEvent)
		void OnRefreshOffset(float PanelHalfHeight);
public:

	void Init(class UInstruction* InstructionInstance);
	UFUNCTION(BlueprintPure)
		FORCEINLINE class UInstruction* GetInstructionInstance() const
	{
		return _InstructionInstance;
	}
	UFUNCTION(BlueprintPure)
		float GetInstructionTime() const;
	UFUNCTION(BlueprintCallable)
		void ClickEventHandler();
	UFUNCTION(BlueprintNativeEvent,meta=(ToolTip="Native Implementation is to show the Detail."))
		void OnWidgetSelected();
	UFUNCTION(BlueprintImplementableEvent)
		void OnWidgetDeselected();
	UFUNCTION(BlueprintImplementableEvent)
		void OnTwikleWidget();
	UFUNCTION(BlueprintCallable)
		void DestroyInstruction();
	UFUNCTION(BlueprintCallable)
		void SetVerticalOffset(float Offset);
	UFUNCTION(BlueprintCallable)
		void RefreshOffset();
};
