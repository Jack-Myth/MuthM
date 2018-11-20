// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DetailsBuilder.h"
#include "HasDetails.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHasDetails : public UInterface
{
	GENERATED_BODY()
};

/**
 * Use To build Details Menu.
 */
class MUTHM_API IHasDetails
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnBuildingDetails(const TScriptInterface<IDetailsBuilder>& DetailsBuilder);
};
