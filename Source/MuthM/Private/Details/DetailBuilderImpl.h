// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Details/DetailBuilder.h"
#include "DetailBuilderImpl.generated.h"

/**
 * 
 */
UCLASS()
class UDetailBuilderImpl : public UObject,public IDetailBuilder
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FName, FDetailCategoryStruct> _DetailCategorylist;
public:
	virtual void AddCategory(FDetailCategoryStruct Category) override;
	virtual UUserWidget* GenDetails() override;

};
