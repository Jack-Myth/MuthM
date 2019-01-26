// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "PauseUIBase.h"
#include "Kismet/GameplayStatics.h"
#include "InGameMode.h"

bool UPauseUIBase::OnGameResumed_Implementation()
{
	RemoveFromParent();
	return true;
}

void UPauseUIBase::Resume()
{
	Cast<AMuthMGameModeBase>(UGameplayStatics::GetGameMode(this))->ResumeGame();
}

void UPauseUIBase::Restart()
{
	AInGameMode* InGameMode;
	InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	if (InGameMode)
		InGameMode->RestartGame();
}

void UPauseUIBase::BackToMenu()
{
	AInGameMode* InGameMode;
	InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
	if (InGameMode)
		InGameMode->ReturnToMainMenuHost();
}
