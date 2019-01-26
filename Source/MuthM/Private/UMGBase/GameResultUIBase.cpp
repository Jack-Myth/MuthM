// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "GameResultUIBase.h"
#include "InGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "InGameState.h"

void UGameResultUIBase::Init()
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	check(InGameMode);
	OnFillScoreInfo(Cast<AInGameState>(UGameplayStatics::GetGameState(this)));
}

void UGameResultUIBase::UIReturnToMainMenu()
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	InGameMode->ReturnToMainMenuHost();
}

void UGameResultUIBase::UIRestartGame()
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	InGameMode->RestartGame();
}
