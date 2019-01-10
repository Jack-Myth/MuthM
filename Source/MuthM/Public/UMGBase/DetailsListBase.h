// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetailsListBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDetailsListClosed,class UDetailsListBase*);

/**
 * 
 */
UCLASS()
class MUTHM_API UDetailsListBase : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnDetailsListClosed OnDetailsListClosed;
	UFUNCTION(BlueprintImplementableEvent)
		void AddCategoryWidget(class UDetailCategoryBase* DetailCategory);
	UFUNCTION(BlueprintCallable)
		void CloseDetails();
	UFUNCTION(BlueprintImplementableEvent)
		void OnDetailsClosed();
};
