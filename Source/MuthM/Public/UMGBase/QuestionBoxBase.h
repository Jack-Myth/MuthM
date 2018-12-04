// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestionBoxBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestionBoxReplied);

/**
 * 
 */
UCLASS()
class MUTHM_API UQuestionBoxBase : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnQuestionBoxReplied OnYes;
	FOnQuestionBoxReplied OnNo;
	FOnQuestionBoxReplied OnCancel;
	UFUNCTION(BlueprintImplementableEvent)
		void OnPupopQuestion(const FText& Question, const FText& Title,bool AllowCancel);
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Select Yes and close the box"))
		void Yes();
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Select No and close the Box"))
		void No();
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Select Cancel and close the box"))
		void Cancel();
};
