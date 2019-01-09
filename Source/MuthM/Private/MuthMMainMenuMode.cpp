// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMMainMenuMode.h"
#include "UIProvider.h"
#include "WelcomeUIBase.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameInstance.h"
#include "ScoreSelectionUIBase.h"
#include "MusicManager.h"
#include "MuthMType.h"

void AMuthMMainMenuMode::LoadWelcome()
{
	auto WelcomeUIClass = UUIProvider::Get(this)->GetWelcomeUI();
	auto* WelcomeUI = Cast<UWelcomeUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), WelcomeUIClass, NAME_None));
	WelcomeUI->AddToViewport();
}

void AMuthMMainMenuMode::OnGameScoreSelected(FScoreSelectionInfo ScoreSelectionInfo)
{
	//Fill GameArg for GameInstance
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	FGameArgs newGameArgs;
	newGameArgs.bIsEditorMode = false;
	newGameArgs.MDATFilePath = ScoreSelectionInfo.MDATPath;
	FMusicInfo MusicInfo;
	IMusicManager::Get(this)->FindMusicLocalByID(ScoreSelectionInfo.MusicID, MusicInfo);
	newGameArgs.MainMusicInfo = MusicInfo;
	newGameArgs.MMSFileName = ScoreSelectionInfo.MMSPath;
	newGameArgs.ScoreIndex = ScoreSelectionInfo.ScoreIndex;
	newGameArgs._MDAT = MakeShareable(new FMDATFile());
	newGameArgs._MDAT->LoadFromFile(ScoreSelectionInfo.MDATPath);
	GameInstance->FillGameArgs(newGameArgs);
}

void AMuthMMainMenuMode::OnEditorScoreSelected(FScoreSelectionInfo ScoreSelectionInfo)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	FGameArgs newGameArgs;
	newGameArgs.bIsEditorMode = true;
	newGameArgs.MDATFilePath = ScoreSelectionInfo.MDATPath;
	FMusicInfo MusicInfo;
	IMusicManager::Get(this)->FindMusicLocalByID(ScoreSelectionInfo.MusicID, MusicInfo);
	newGameArgs.MainMusicInfo = MusicInfo;
	newGameArgs.MMSFileName = ScoreSelectionInfo.MMSPath;
	newGameArgs.ScoreIndex = ScoreSelectionInfo.ScoreIndex;
	newGameArgs._MDAT = MakeShareable(new FMDATFile());
	newGameArgs._MDAT->LoadFromFile(ScoreSelectionInfo.MDATPath);
	GameInstance->FillGameArgs(newGameArgs);
}

void AMuthMMainMenuMode::OnScoreSelectionCanceled()
{
	//TODO: Clear Game Args,no use for now.
}

void AMuthMMainMenuMode::BeginPlay()
{
	UGameplayStatics::GetPlayerController(this, 0)->bShowMouseCursor=true;
	auto* GameInstance =  Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	FString _MDATFile;
	int ScoreIndex;
	bool bIsEditorMode;
	GameInstance->GetLastScoreInfo(_MDATFile,ScoreIndex,bIsEditorMode);
	if (_MDATFile!="")
	{
		// Need to restore Selection UI;
		if (bIsEditorMode)
			SelectEditorScore()->OnRestoreScoreSelection(_MDATFile, ScoreIndex);
		else
			SelectGameScore()->OnRestoreScoreSelection(_MDATFile, ScoreIndex);
	}
	else
	{
		// At the first time start the Game.
		LoadWelcome();
	}
}

class UScoreSelectionUIBase* AMuthMMainMenuMode::SelectGameScore()
{
	auto ScoreSelectionUIClass = UUIProvider::Get(this)->GetScoreSelectionUI();
	auto* ScoreSelectionUI = Cast<UScoreSelectionUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), ScoreSelectionUIClass, NAME_None));
	ScoreSelectionUI->OnScoreSelected.AddDynamic(this, &AMuthMMainMenuMode::OnGameScoreSelected);
	ScoreSelectionUI->OnSelectionCancelled.AddDynamic(this, &AMuthMMainMenuMode::OnScoreSelectionCanceled);
	ScoreSelectionUI->AddToViewport(101);
	return ScoreSelectionUI;
	//TODO:Bind Delegate.
}

class UScoreSelectionUIBase* AMuthMMainMenuMode::SelectEditorScore()
{
	auto ScoreSelectionUIClass = UUIProvider::Get(this)->GetScoreSelectionUI();
	auto* ScoreSelectionUI = Cast<UScoreSelectionUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), ScoreSelectionUIClass, NAME_None));
	ScoreSelectionUI->OnScoreSelected.AddDynamic(this, &AMuthMMainMenuMode::OnEditorScoreSelected);
	ScoreSelectionUI->OnSelectionCancelled.AddDynamic(this, &AMuthMMainMenuMode::OnScoreSelectionCanceled);
	ScoreSelectionUI->AddToViewport(101);
	return ScoreSelectionUI;
	//TODO: Bind Delegate
}
