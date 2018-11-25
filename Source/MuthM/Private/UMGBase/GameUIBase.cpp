// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "GameUIBase.h"
#include "Kismet/GameplayStatics.h"
#include "Score/ScoreCore.h"
#include "MuthMInEditorMode.h"
#include "MuthMInGameMode.h"

void UGameUIBase::Init(FMusicInfo MusicInfo)
{
	OnFillMusicInfo(MusicInfo);
	auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
	//Register Delegate
	if (InGameMode)
	{
		InGameMode->GetScoreCore()->OnScoreUpdate.AddDynamic(this, &UGameUIBase::OnScoreChanged);
		InGameMode->GetScoreCore()->OnComboUpdate.AddDynamic(this, &UGameUIBase::OnComboUpdate);
		InGameMode->GetScoreCore()->OnScoreGradeUpdate.AddDynamic(this, &UGameUIBase::OnScoreGradeUpdate);
		InGameMode->OnMusicPlaybackTimeUpdate.AddDynamic(this, &UGameUIBase::OnGameProgressChanged);
	}
}

void UGameUIBase::ExitPIE()
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	if (InEditorMode)
		InEditorMode->ExitPIE();
}

void UGameUIBase::ShowPauseUI()
{
	auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
	if (InGameMode)
		InGameMode->PauseGame();
}
