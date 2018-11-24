// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MusicManager.h"
#include "GameUIBase.generated.h"

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
	UFUNCTION(BlueprintCallable)
		void OnGameProgressChanged(float NewProgressPercent);
	UFUNCTION(BlueprintImplementableEvent)
		void OnScoreChanged(float CurScore, float RealScore);
};
