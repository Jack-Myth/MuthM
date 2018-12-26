// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputBoxBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputBoxInput,const FString&,InputString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputBoxCancelled);

/**
 * 
 */
UCLASS()
class MUTHM_API UInputBoxBase : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnInputBoxInput OnInput;
	FOnInputBoxCancelled OnCancelled;
	UFUNCTION(BlueprintCallable)
		void Input(const FString& InputString);
	UFUNCTION(BlueprintCallable)
		void Cancelled();
	UFUNCTION(BlueprintImplementableEvent)
		void OnPupopInput(const FText& Title, const FText& Message, const FString& DefaultInput,bool AllowCancel);
};
