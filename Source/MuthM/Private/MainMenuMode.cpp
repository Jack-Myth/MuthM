// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainMenuMode.h"
#include "UIProvider.h"
#include "WelcomeUIBase.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameInstance.h"
#include "ScoreSelectionUIBase.h"
#include "MusicManager.h"
#include "MuthMType.h"
#include "MainMenuUIBase.h"
#include "MuthMTypeHelper.h"
#include "Paths.h"

DEFINE_LOG_CATEGORY(MainMenuMode)

void AMainMenuMode::OnGameScoreSelected(const FString& MDATFileName, int ScoreIndex)
{
	//Fill GameArg for GameInstance
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	FGameArgs newGameArgs;
	newGameArgs.bIsEditorMode = false;
	newGameArgs.MDATFileName = MDATFileName;
	newGameArgs._MDAT = MakeShareable(new FMDATFile());
	newGameArgs._MDAT->LoadFromFile(FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("/MDATs/"), MDATFileName));
	FMDATMainInfo tmpMDATMainInfo;
	if (MuthMTypeHelper::TryGetMDATMainInfo(newGameArgs._MDAT.Get(), tmpMDATMainInfo))
	{
		FMusicInfo MusicInfo;
		IMusicManager::Get(this)->FindMusicLocalByID(tmpMDATMainInfo.ScoreInfoCollection[ScoreIndex].MusicID, MusicInfo);
		newGameArgs.MainMusicInfo = MusicInfo;
		newGameArgs.MMSFileName = tmpMDATMainInfo.ScoreInfoCollection[ScoreIndex].ScoreDataFileName;
		newGameArgs.ScoreIndex = ScoreIndex;
		GameInstance->FillGameArgs(newGameArgs);
		UGameplayStatics::OpenLevel(this, "Game");
	}
}

void AMainMenuMode::OnScoreSelectionCanceled()
{
	//TODO: Clear Game Args,no use for now.
}

void AMainMenuMode::BeginPlay()
{
	UGameplayStatics::GetPlayerController(this, 0)->bShowMouseCursor=true;
	auto* GameInstance =  Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	FString _MDATFile;
	int ScoreIndex;
	bool bIsEditorMode;
	GameInstance->GetLastScoreInfo(_MDATFile,ScoreIndex,bIsEditorMode);
	if (_MDATFile!="")
	{
		//Load MainMenu directly
		auto MainMenuUIClass = UUIProvider::Get(this)->GetMainMenuUI();
		auto* MainMenuUI = Cast<UMainMenuUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), MainMenuUIClass, NAME_None));
		if (MainMenuUI)
			MainMenuUI->AddToViewport(100);
		else
			UE_LOG(MainMenuMode, Error, TEXT("Failed to construct MainMenuUI!"));

		// Need to restore Selection UI;
		if (!bIsEditorMode)
			SelectGameScore()->OnRestoreScoreSelection(_MDATFile, ScoreIndex);
	}
	else
	{
		//At the first, load WelcomeUI
		auto WelcomeUIClass = UUIProvider::Get(this)->GetWelcomeUI();
		auto* WelcomeUI = Cast<UWelcomeUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), WelcomeUIClass, NAME_None));
		WelcomeUI->AddToViewport();
	}
}

class UScoreSelectionUIBase* AMainMenuMode::SelectGameScore()
{
	auto ScoreSelectionUIClass = UUIProvider::Get(this)->GetScoreSelectionUI();
	auto* ScoreSelectionUI = Cast<UScoreSelectionUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), ScoreSelectionUIClass, NAME_None));
	ScoreSelectionUI->OnScoreSelected.BindUObject(this, &AMainMenuMode::OnGameScoreSelected);
	ScoreSelectionUI->AddToViewport(101);
	return ScoreSelectionUI;
	//TODO:Bind Delegate.
}