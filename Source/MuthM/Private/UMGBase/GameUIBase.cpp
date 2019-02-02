// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "GameUIBase.h"
#include "Kismet/GameplayStatics.h"
#include "Score/InGameState.h"
#include "InEditorMode.h"
#include "InGameMode.h"
#include "MuthMGameInstance.h"
#include "InPIEMode.h"

//DEFINE_LOG_CATEGORY(GameUIBase)

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

	InitUI(InGameMode->IsA(AInPIEMode::StaticClass()));
}

void UGameUIBase::ExitPIE()
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameInstance->ExitPIEMode();
}

void UGameUIBase::ShowPauseUI()
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	if (InGameMode)
		InGameMode->PauseGame();
}
