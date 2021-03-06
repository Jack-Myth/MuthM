// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GenericSelectionItemBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGSIOnSelected, class UGenericSelectionItemBase*, ItemRef,int,DataIndex);

/**
 * 
 */
UCLASS()
class MUTHM_API UGenericSelectionItemBase : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly,BlueprintGetter="GetDataIndex")
		int _DataIndex;
public:
	UPROPERTY(BlueprintAssignable)
		FGSIOnSelected OnSelected;
	UFUNCTION(BlueprintCallable)
		void SetDataIndex(int newDataIndex)
	{
		_DataIndex = newDataIndex;
	}
	UFUNCTION(BlueprintGetter)
	FORCEINLINE int GetDataIndex()
	{
		return _DataIndex;
	}
	UFUNCTION(BlueprintCallable)
		void NotifySelected();
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
		void Deselected();
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent,meta=(ToolTip="MinjorText and Icon May be empty,check it and use different appearance."))
		void ApplyAppearance(const FText& Title, const FText& Subtitle, class UTexture2D* Icon);
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent,meta=(ToolTip="Brightness mostly between 0-1,0 means black ,1 means normal."))
		void ChangeIconBrightness(float Brightness);
};
