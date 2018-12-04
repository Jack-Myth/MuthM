// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMMainMenuMode.h"
#include "UIProvider.h"


void AMuthMMainMenuMode::LoadWelcomeUI()
{
	auto WelcomeUIClass = UUIProvider::Get()->GetWelcomeUI();
	auto* WelcomeUI = Cast<UWelcomeUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), WelcomeUIClass, "WelcomeUI"));
	if (WelcomeUI)
		WelcomeUI->AddToViewport(100);
}
