// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultUIBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UGameResultUIBase : public UUserWidget
{
	GENERATED_BODY()

	class AInGameState* _ScoreSource;
public:
	void Init();
	UFUNCTION(BlueprintImplementableEvent)
		void OnFillScoreInfo(class AInGameState* ScoreSource);
	UFUNCTION(BlueprintCallable)
		void UIReturnToMainMenu();
	UFUNCTION(BlueprintCallable)
		void UIRestartGame();
};
