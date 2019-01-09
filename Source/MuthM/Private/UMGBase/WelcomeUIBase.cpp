// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "WelcomeUIBase.h"
#include "UIProvider.h"
#include "MainMenuUIBase.h"

bool UWelcomeUIBase::FindAndLoadMods()
{
	//UNDONE: Find And Load Mods
	return false;
}

void UWelcomeUIBase::LoadMainMenuUI()
{
		auto MainMenuUIClass = UUIProvider::Get(this)->GetMainMenuUI();
		auto* MainMenuUI = Cast<UMainMenuUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), MainMenuUIClass, NAME_None));
		if (MainMenuUI)
			MainMenuUI->AddToViewport(100);

}
