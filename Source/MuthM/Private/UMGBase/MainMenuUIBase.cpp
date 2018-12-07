// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainMenuUIBase.h"
#include "UIProvider.h"
#include "MuthMMainMenuMode.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuUIBase::PendingStartGame()
{
	auto* MainMenuMode = Cast<AMuthMMainMenuMode>(UGameplayStatics::GetGameMode(this));
	MainMenuMode->SelectGameScore();
}
