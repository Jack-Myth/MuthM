// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainMenuUIBase.h"
#include "UIProvider.h"

void UMainMenuUIBase::PendingStartGame()
{
	auto ScoreSelectionUIClass = UUIProvider::Get()->GetScoreSelectionUI();
	auto* ScoreSelectionUI=Cast<UScoreSelectionUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), ScoreSelectionUIClass, "ScoreSelectionUI"));
	ScoreSelectionUI->AddToViewport();
}
