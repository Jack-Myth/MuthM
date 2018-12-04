// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageBoxBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMessageBoxConfirmed);

/**
 * 
 */
UCLASS()
class MUTHM_API UMessageBoxBase : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnMessageBoxConfirmed OnMessageBoxConfirmed;
	UFUNCTION(BlueprintImplementableEvent)
		void OnPupopMessage(const FText& Message,const FText& Title);
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Confirm and close the messagebox"))
		void Confirm();
};
