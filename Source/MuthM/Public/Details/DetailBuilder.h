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
	/*UFUNCTION(BlueprintCallable)
		virtual void AddCategory(FDetailCategory Category) = 0;*/
};
