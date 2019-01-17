// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainMenuUIBase.h"
#include "UIProvider.h"
#include "MainMenuMode.h"
#include "Kismet/GameplayStatics.h"
#include "ContentManagerUIBase.h"
#include "ScoreEditorEntryUIBase.h"

void UMainMenuUIBase::PendingStartGame()
{
	auto* MainMenuMode = Cast<AMainMenuMode>(UGameplayStatics::GetGameMode(this));
	MainMenuMode->SelectGameScore();
}

void UMainMenuUIBase::ShowContentManagerUI()
{
	auto CMUIClass = UUIProvider::Get(this)->GetContentManagerUI();
	auto* CMUIInstance = Cast<UContentManagerUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), CMUIClass, NAME_None));
	CMUIInstance->AddToViewport(101);
}

void UMainMenuUIBase::ShowScoreEditorEntryUI()
{
	auto SEEUIClass = UUIProvider::Get(this)->GetScoreEditorEntryUI();
	auto* SEEUIInstance = Cast<UScoreEditorEntryUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), SEEUIClass, NAME_None));
	SEEUIInstance->AddToViewport(101);
}
