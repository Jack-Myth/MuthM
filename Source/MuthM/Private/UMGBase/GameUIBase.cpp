// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "GameUIBase.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMInGameMode.h"
#include "Score/ScoreCore.h"

void UGameUIBase::Init(FMusicInfo MusicInfo)
{
	OnFillMusicInfo(MusicInfo);
	auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
	//Register Score Delegate
	if (InGameMode)
		InGameMode->GetScoreCore()->OnScoreUpdate.AddDynamic(this, &UGameUIBase::OnScoreChanged);
}
