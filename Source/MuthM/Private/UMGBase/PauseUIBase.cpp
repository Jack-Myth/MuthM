// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "PauseUIBase.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMInGameMode.h"

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
	AMuthMInGameMode* InGameMode;
	InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
	if (InGameMode)
		InGameMode->RestartGame();
}

void UPauseUIBase::BackToMenu()
{
	//UNDONE:BackToMenu() Maybe it's need some delay?
}
