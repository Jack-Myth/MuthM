// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUIBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UPauseUIBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, meta=(ToolTip="This event may fire many times if OnGameResumed return false."))
		void OnGamePaused();
	UFUNCTION(BlueprintNativeEvent,meta=(ToolTip="Return true if the PauseUI is about to be removed."))
		bool OnGameResumed();

	UFUNCTION(BlueprintCallable)
		void Resume();
	UFUNCTION(BlueprintCallable)
		void Restart();
	UFUNCTION(BlueprintCallable)
		void BackToMenu();
};
