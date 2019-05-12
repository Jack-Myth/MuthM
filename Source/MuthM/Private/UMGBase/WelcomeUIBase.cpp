// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "WelcomeUIBase.h"
#include "UIProvider.h"
#include "MainMenuUIBase.h"
#include "ModManager.h"

bool UWelcomeUIBase::FindAndLoadMods()
{
	IModManager::Get(this)->InitLoadMods();
	return true;
}

void UWelcomeUIBase::LoadMainMenuUI()
{
		auto MainMenuUIClass = UUIProvider::Get(this)->GetMainMenuUI();
		auto* MainMenuUI = Cast<UMainMenuUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), MainMenuUIClass, NAME_None));
		if (MainMenuUI)
			MainMenuUI->AddToViewport(100);

}
