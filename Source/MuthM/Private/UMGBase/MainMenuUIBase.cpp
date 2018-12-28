// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainMenuUIBase.h"
#include "UIProvider.h"
#include "MuthMMainMenuMode.h"
#include "Kismet/GameplayStatics.h"
#include "ContentManagerUIBase.h"
#include "ScoreEditorEntryUIBase.h"

void UMainMenuUIBase::PendingStartGame()
{
	auto* MainMenuMode = Cast<AMuthMMainMenuMode>(UGameplayStatics::GetGameMode(this));
	MainMenuMode->SelectGameScore();
}

void UMainMenuUIBase::ShowContentManagerUI()
{
	auto CMUIClass = UUIProvider::Get(this)->GetContentManagerUI();
	auto* CMUIInstance = Cast<UContentManagerUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), CMUIClass, "ContentManagerUI"));
	CMUIInstance->AddToViewport(101);
}

void UMainMenuUIBase::ShowScoreEditorEntryUI()
{
	auto SEEUIClass = UUIProvider::Get(this)->GetScoreEditorEntryUI();
	auto* SEEUIInstance = Cast<UScoreEditorEntryUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), SEEUIClass, "ScoreEditorEntryUI"));
	SEEUIInstance->AddToViewport(101);
}
