// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DetailStructures.h"
#include "DetailBuilder.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UDetailBuilder : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for blueprint to construct details menu.
 */
class MUTHM_API IDetailBuilder
{
	GENERATED_BODY()

public:
	static TScriptInterface<IDetailBuilder> GenNewBuilder();
	UFUNCTION(BlueprintCallable)
		virtual void AddCategory(FDetailCategoryStruct Category) = 0;
	virtual UUserWidget* GenDetails() = 0;
};
