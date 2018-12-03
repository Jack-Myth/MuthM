// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EditorPanelBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UEditorPanelBase : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
		class UTextureRenderTarget2D* _SpectrumRenderTarget;
	UPROPERTY()
		TArray<class UInstructionWidgetBase*> InstructionWidgets;
	UPROPERTY()
		class UInstructionWidgetBase* _SelectedWidget;
	bool _NextToAdd;
protected:
	UPROPERTY(BlueprintReadOnly)
		float PanelLBorder;
	UPROPERTY(BlueprintReadOnly)
		float PanelRBorder;

	//default widget(R-L)=30 seconds
	UPROPERTY(BlueprintReadOnly)
		float ScaleRatio;
	UPROPERTY(BlueprintReadOnly)
		float TimeAxis;
public:
	UPROPERTY(BlueprintReadWrite)
		float _BPM;
	UPROPERTY(BlueprintReadWrite)
		float _AlignOffset;
	UPROPERTY(BlueprintReadWrite)
		int BeatDenominator = 4;
	UPROPERTY(BlueprintReadWrite)
		bool bShouldAlignBPM;
	UPROPERTY(BlueprintReadWrite)
		bool bFastAddMode = false;
	UPROPERTY(BlueprintReadWrite)
		FName _FastAddInstructionName;
	UFUNCTION(BlueprintCallable)
		void SetSpectrumRes(int ResX, int ResY);
	UFUNCTION(BlueprintCallable)
		void RefreshSpectrum();
	UFUNCTION(BlueprintImplementableEvent)
		void OnSpectrumUpdate(class UTextureRenderTarget2D* Spectrum);
	UFUNCTION(BlueprintCallable)
		void SetTimelineScale(float NewScaleRatio);
	UFUNCTION(BlueprintImplementableEvent)
		void OnPanelBoundChanged(float LBorder,float RBorder);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionWidgetAdded(class UInstructionWidgetBase* InstructionWidget);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionWidgetTimeChanged(class UInstructionWidgetBase* InstructionWidget);
	UFUNCTION(BlueprintCallable)
		void SetTimeAxis(float NewTime);
	UFUNCTION(BlueprintImplementableEvent)
		void OnTimeAxisChanged(float CurrentTime);
	UFUNCTION(BlueprintPure)
		FORCEINLINE class UInstructionWidgetBase* GetSelectedWidget()
	{
		return _SelectedWidget;
	}
	//Bind To Instruction Widget's onClick Event
	//To Handle all Instruction's behavior.
	void ClickWidget(class UInstructionWidgetBase* newClickedWidget);
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Use to handle the click(or touch) event on Panel"))
		void OnClickHandler(float Time);
	UFUNCTION(BlueprintPure)
		inline void GetAlignBPMInfo(float& AlignOffset, float& AlignTime)
	{
		AlignOffset = _AlignOffset;
		AlignTime = 60.f / _BPM / BeatDenominator;
	}
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionSelected(class UInstructionWidgetBase* SelectedWidget);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionDeselected(class UInstructionWidgetBase* DeselectedWidget);
	UFUNCTION(BlueprintCallable)
		void RemoveInstruction(class UInstructionWidgetBase* WidgetToRemove);
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Set if Next click on panel will add an instruction widget"))
		FORCEINLINE void SetNextAdd(bool newNextAdd)
	{
		_NextToAdd = newNextAdd;
	}
protected:
	virtual void NativeConstruct() override;

};
