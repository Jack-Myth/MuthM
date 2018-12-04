// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUIBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UMainMenuUIBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		bool FindAndLoadMods();
	UFUNCTION(BlueprintCallable)
		void PendingStartGame();
};
