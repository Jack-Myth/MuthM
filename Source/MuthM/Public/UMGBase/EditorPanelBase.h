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
	float _BPM;
	float _AlignOffset;
	bool bShouldAlignBPM;
	bool bFastAddMode = false;
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
	void ClickWidget(class UInstructionWidgetBase* newClickedWidget);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetIsFastAddMode(bool newIsFastAdd)
	{
		bFastAddMode = newIsFastAdd;
	}
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsInFastAddMode()
	{
		return bFastAddMode;
	}
	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetAlignBPM(bool newAlighBPM)
	{
		bShouldAlignBPM = newAlighBPM;
	}
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool ShouldAlighBPM()
	{
		return bShouldAlignBPM;
	}
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Use to handle the click(or touch) event on Panel"))
		void OnClickHandler(float Time, float Y);
	void GetAlignBPMInfo(float AlignOffset,)
protected:
	virtual void NativeConstruct() override;

};
