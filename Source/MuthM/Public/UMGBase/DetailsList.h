// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailsList.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UDetailsList : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		void AddCategoryWidget(class UDetailCategory* DetailCategory);
};
