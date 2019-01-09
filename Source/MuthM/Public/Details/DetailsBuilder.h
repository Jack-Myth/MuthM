// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DetailStructures.h"
#include "DetailsBuilder.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UDetailsBuilder : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for blueprint to construct details menu.
 */
class MUTHM_API IDetailsBuilder
{
	GENERATED_BODY()

public:
	static TScriptInterface<IDetailsBuilder> GenNewBuilder();
	UFUNCTION(BlueprintCallable)
		virtual void AddCategory(FDetailCategoryStruct Category) = 0;
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Notice:This function will not merge the same category."))
		virtual void AddCategoryBefore(FDetailCategoryStruct Category, FName Before) = 0;
	virtual FDetailCategoryStruct* FindCategoryByName(FName CategoryName) = 0;
	virtual class UDetailsListBase* GenDetailsWidget() = 0;
	virtual void CollectDetails() = 0;
	virtual void SetDetailsHolder(TScriptInterface<class IHasDetails> _DetailsHolder) = 0;
};
