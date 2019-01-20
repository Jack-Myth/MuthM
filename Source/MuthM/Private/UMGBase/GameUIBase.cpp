// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "GameUIBase.h"
#include "Kismet/GameplayStatics.h"
#include "Score/InGameState.h"
#include "InEditorMode.h"
#include "InGameMode.h"

void UGameUIBase::Init(FMusicInfo MusicInfo)
{
	OnFillMusicInfo(MusicInfo);
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	auto* InGameState = Cast<AInGameState>(UGameplayStatics::GetGameState(this));
	//Register Delegate
	if (InGameMode)
	{
		if (InGameState)
		{
			InGameState->OnScoreUpdate.AddDynamic(this, &UGameUIBase::OnScoreChanged);
			InGameState->OnComboUpdate.AddDynamic(this, &UGameUIBase::OnComboUpdate);
			InGameState->OnScoreGradeUpdate.AddDynamic(this, &UGameUIBase::OnScoreGradeUpdate);
		}
		InGameMode->OnMusicPlaybackTimeUpdate.AddDynamic(this, &UGameUIBase::OnGameProgressChanged);
	}
}

void UGameUIBase::ExitPIE()
{
	auto* InEditorMode = Cast<AInEditorMode>(UGameplayStatics::GetGameMode(this));
	if (InEditorMode)
		InEditorMode->NativeOnExitPIE();
}

void UGameUIBase::ShowPauseUI()
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	if (InGameMode)
		InGameMode->PauseGame();
}
