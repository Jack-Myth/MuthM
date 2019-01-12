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

	friend class UEditorMainUIBase;
	UPROPERTY()
		TArray<class UTexture2D*> _SpectrumTexture;
	UPROPERTY()
		TArray<class UInstructionWidgetBase*> InstructionWidgets;
	UPROPERTY()
		class UInstructionWidgetBase* _SelectedWidget=nullptr;
	UPROPERTY()
		class UDetailsListBase* ActivedDetailsWidget=nullptr;
	bool _NextToAdd;
protected:
	//default widget(R-L)=30 seconds
	UPROPERTY(BlueprintReadWrite)
		float ScaleRatio=1.f;
	UPROPERTY(BlueprintReadOnly)
		float TimeAxis;
	UFUNCTION(BlueprintImplementableEvent, meta = (ToolTip = "Notice:This event may be fired before construct event."))
		void OnSpectrumUpdate(const TArray<class UTexture2D*>& Spectrum);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionWidgetAdded(class UInstructionWidgetBase* InstructionWidget);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionWidgetTimeChanged(class UInstructionWidgetBase* InstructionWidget, float LastTime, float CurrentTime);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionSelected(class UInstructionWidgetBase* SelectedWidget);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionDeselected(class UInstructionWidgetBase* DeselectedWidget);
	UFUNCTION()
		void OnEditorPropertyInput(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
	UFUNCTION(BlueprintCallable,meta=(ToolTip="VerticalOffset should between -1 and 1"))
		void AddInstructionAtTime(float Time,float VerticalOffset);
	UFUNCTION()
		void OnDetailListClosed(class UDetailsListBase* DetailsListWidget);
	UPROPERTY(BlueprintReadWrite)
		bool bIgnoreVerticalPosition=true;
	UFUNCTION(BlueprintImplementableEvent)
		void OnVerticalOffsetUpdate(class UInstructionWidgetBase* TargetWidget,float Value);
	UFUNCTION(BlueprintImplementableEvent)
		void OnFillBPMInfo(float BPM);
public:

	void FillBPMInfo(float BPM);

	UPROPERTY(BlueprintReadWrite)
		class UInstruction* InstructionTemplate = nullptr;

	UPROPERTY(BlueprintReadWrite)
		float _BPM;
	UPROPERTY(BlueprintReadWrite)
		float _AlignOffset;
	UPROPERTY(BlueprintReadWrite)
		int _BeatDenominator = 4;
	UPROPERTY(BlueprintReadWrite)
		bool bShouldAlignBPM;
	UPROPERTY(BlueprintReadWrite)
		bool bFastAddMode = false;
	UPROPERTY(BlueprintReadWrite)
		FName _FastAddInstructionName;
	//UFUNCTION(BlueprintCallable)
		//class UImage* ConstructImageWidget();
	UFUNCTION(BlueprintPure)
	FORCEINLINE TArray<UInstructionWidgetBase*> GetInstructionWidgets()
	{
		return InstructionWidgets;
	}

	UFUNCTION(BlueprintCallable)
		TArray<UInstructionWidgetBase*> GetOrderedInstructionWidgets();
	UFUNCTION(BlueprintCallable)
		void SetTimeAxis(float NewTime);
	UFUNCTION(BlueprintImplementableEvent)
		void OnTimeAxisChanged(float CurrentTime);
	UFUNCTION(BlueprintImplementableEvent)
		void OnCenterTimeAxis();
	UFUNCTION(BlueprintPure)
		FORCEINLINE class UInstructionWidgetBase* GetSelectedWidget()
	{
		return _SelectedWidget;
	}
	//Bind To Instruction Widget's onClick Event
	//To Handle all Instruction's behavior.
	void ClickWidget(class UInstructionWidgetBase* newClickedWidget);
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Use to handle the click(or touch) event on Panel\nVerticalOffset should between -1 and 1"))
		void OnClickHandler(float Time,float VerticalOffset);
	UFUNCTION(BlueprintPure)
	void GetAlignBPMInfo(float& AlignOffset, float& AlignTime)
	{
		AlignOffset = _AlignOffset;
		AlignTime = 60.f / _BPM / _BeatDenominator;
	}
	UFUNCTION(BlueprintCallable)
		void RemoveInstruction(class UInstructionWidgetBase* WidgetToRemove);
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Set if Next click on panel will add an instruction widget"))
	void SetNextAdd(bool newNextAdd)
	{
		_NextToAdd = newNextAdd;
	}
	UFUNCTION(BlueprintCallable)
		void DeleteCurrentInstruction();
	UFUNCTION(BlueprintPure)
		float GetMusicLength();
	UFUNCTION(BlueprintCallable)
		void PupopDetails(class UInstructionWidgetBase* InstructionWidgetBase);
	UFUNCTION(BlueprintCallable)
		void PupopTemplateDetails();
	UFUNCTION()
		void OnMusicProcessCallback(float Current, float Duration);
	UFUNCTION(BlueprintCallable)
		void SetInstructionTemplateByName(FName TemplateInstructionName);
	UFUNCTION(BlueprintImplementableEvent,BlueprintPure)
		float GetPanelHalfHeight() const;
protected:
	virtual void NativeConstruct() override;

};
