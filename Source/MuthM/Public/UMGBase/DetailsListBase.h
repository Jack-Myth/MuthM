// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailsListBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UDetailsListBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		void AddCategoryWidget(class UDetailCategoryBase* DetailCategory);
};
