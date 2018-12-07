// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMMainMenuMode.h"
#include "UIProvider.h"
#include "WelcomeUIBase.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameInstance.h"
#include "ScoreSelectionUIBase.h"

void AMuthMMainMenuMode::LoadWelcome()
{
	auto WelcomeUIClass = UUIProvider::Get()->GetWelcomeUI();
	auto* WelcomeUI = Cast<UWelcomeUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), WelcomeUIClass, "WelcomeUI"));
	WelcomeUI->AddToViewport();
}

void AMuthMMainMenuMode::BeginPlay()
{
	UGameplayStatics::GetPlayerController(this, 0)->bShowMouseCursor=true;
	auto* GameInstance =  Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	FString MDATFile;
	int ScoreIndex;
	bool bIsEditorMode;
	GameInstance->GetLastScoreInfo(MDATFile,ScoreIndex,bIsEditorMode);
	if (MDATFile!="")
	{
		// Need to restore Selection UI;
		if (bIsEditorMode)
			SelectEditorScore()->OnRestoreScoreSelection(MDATFile, ScoreIndex);
		else
			SelectGameScore()->OnRestoreScoreSelection(MDATFile, ScoreIndex);
	}
	else
	{
		// At the first time start the Game.
		LoadWelcome();
	}
}

class UScoreSelectionUIBase* AMuthMMainMenuMode::SelectGameScore()
{
	auto ScoreSelectionUIClass = UUIProvider::Get()->GetScoreSelectionUI();
	auto* ScoreSelectionUI = Cast<UScoreSelectionUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), ScoreSelectionUIClass, "ScoreSelectionUI"));
	ScoreSelectionUI->AddToViewport();
	return ScoreSelectionUI;
	//TODO:Bind Delegate.
}

class UScoreSelectionUIBase* AMuthMMainMenuMode::SelectEditorScore()
{
	auto ScoreSelectionUIClass = UUIProvider::Get()->GetScoreSelectionUI();
	auto* ScoreSelectionUI = Cast<UScoreSelectionUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), ScoreSelectionUIClass, "ScoreSelectionUI"));
	ScoreSelectionUI->AddToViewport();
	return ScoreSelectionUI;
	//TODO: Bind Delegate
}
