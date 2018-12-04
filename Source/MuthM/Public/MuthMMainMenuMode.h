// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MuthMGameModeBase.h"
#include "MuthMMainMenuMode.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API AMuthMMainMenuMode : public AMuthMGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void LoadWelcomeUI();
};
