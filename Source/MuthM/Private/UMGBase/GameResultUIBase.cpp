// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "GameResultUIBase.h"
#include "MuthMInGameMode.h"
#include "Kismet/GameplayStatics.h"

void UGameResultUIBase::Init()
{
	auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
	check(InGameMode);
	OnFillScoreInfo(InGameMode->GetScoreCore());
}

void UGameResultUIBase::UIReturnToMainMenu()
{
	auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
	InGameMode->ReturnToMainMenu();
}

void UGameResultUIBase::UIRestartGame()
{
	auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
	InGameMode->RestartGame();
}
