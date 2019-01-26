// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MusicManager.h"
#include "GameUIBase.generated.h"

//DECLARE_LOG_CATEGORY_EXTERN(GameUIBase, Log, All)

/**
 * 
 */
UCLASS()
class MUTHM_API UGameUIBase : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(FMusicInfo MusicInfo);
	UFUNCTION(BlueprintImplementableEvent)
		void OnFillMusicInfo(FMusicInfo MusicInfo);
	UFUNCTION(BlueprintImplementableEvent)
		void OnGameProgressChanged(float CurrentTime,float Duration);
	UFUNCTION(BlueprintImplementableEvent)
		void OnScoreChanged(float CurScore, float RealScore);
	UFUNCTION(BlueprintImplementableEvent)
		void OnComboUpdate(int CurrentCombo);
	UFUNCTION(BlueprintImplementableEvent)
		void OnScoreGradeUpdate(EScoreGrade NewScoreGrade);
	UFUNCTION(BlueprintImplementableEvent)
		void InitUI(bool IsPIEMode);
	UFUNCTION(BlueprintCallable)
		void ExitPIE();
	UFUNCTION(BlueprintCallable)
		void ShowPauseUI();
};
