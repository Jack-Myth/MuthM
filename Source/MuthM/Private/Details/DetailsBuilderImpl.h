// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Details/DetailsBuilder.h"
#include "DetailsBuilderImpl.generated.h"

/**
 * 
 */
UCLASS()
class UDetailsBuilderImpl : public UObject,public IDetailsBuilder
{
	GENERATED_BODY()

	UPROPERTY()
		TScriptInterface<class IHasDetails> DetailsHolder;		//the Details Holder should provider the UWorld for Details Builder.

	UPROPERTY()
	TArray<FDetailCategoryStruct> _DetailCategorylist;
public:
	virtual void AddCategory(FDetailCategoryStruct Category) override;
	virtual class UDetailsListBase* GenDetails() override;
	virtual class UWorld* GetWorld() const override;
	virtual void SetDetailsHolder(TScriptInterface<class IHasDetails> _DetailsHolder) override;
	virtual void AddCategoryBefore(FDetailCategoryStruct Category, FName Before) override;

};
