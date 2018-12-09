// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailCategoryBase.generated.h"

/**
 * A Holder of Detail Input.
 * Just For Visual, less logic.
 */
UCLASS()
class MUTHM_API UDetailCategoryBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		void SetHeaderText(const FText& HeaderText);
	UFUNCTION(BlueprintImplementableEvent)
		void AddDetailItemWidget(UUserWidget* DetailItem);
};
