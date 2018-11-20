// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailCategory.generated.h"

/**
 * A Holder of Detail Input.
 * Just For Visual, less logic.
 */
UCLASS()
class MUTHM_API UDetailCategory : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		void SetHeaderText(const FString& HeaderText);
	UFUNCTION(BlueprintImplementableEvent)
		void AddDetailItemWidget(UUserWidget* DetailItem);
};
