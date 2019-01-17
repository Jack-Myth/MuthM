// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MuthMGameModeBase.h"
#include "ScoreSelectionUIBase.h"
#include "MainMenuMode.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API AMainMenuMode : public AMuthMGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void LoadWelcome();
	UFUNCTION()
		void OnGameScoreSelected(FScoreSelectionInfo ScoreSelectionInfo);
	UFUNCTION()
		void OnEditorScoreSelected(FScoreSelectionInfo ScoreSelectionInfo);
	UFUNCTION()
		void OnScoreSelectionCanceled();
protected:
	virtual void BeginPlay() override;
public:
	class UScoreSelectionUIBase* SelectGameScore();
	class UScoreSelectionUIBase* SelectEditorScore();
};
