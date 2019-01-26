// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MuthMGameModeBase.h"
#include "ScoreSelectionUIBase.h"
#include "MainMenuMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MainMenuMode, Log, All);

/**
 * 
 */
UCLASS()
class MUTHM_API AMainMenuMode : public AMuthMGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION()
		void OnGameScoreSelected(const FString& MDATFileName,int ScoreIndex);
	UFUNCTION()
		void OnScoreSelectionCanceled();
protected:
	virtual void BeginPlay() override;
public:
	class UScoreSelectionUIBase* SelectGameScore();
};
