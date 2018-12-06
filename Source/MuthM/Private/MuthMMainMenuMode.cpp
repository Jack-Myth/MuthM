// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMMainMenuMode.h"
#include "UIProvider.h"
#include "WelcomeUIBase.h"

void AMuthMMainMenuMode::LoadWelcome()
{
	auto WelcomeUIClass = UUIProvider::Get()->GetWelcomeUI();
	auto* WelcomeUI = Cast<UWelcomeUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), WelcomeUIClass, "WelcomeUI"));
	WelcomeUI->AddToViewport();
}

void AMuthMMainMenuMode::BeginPlay()
{
	
}
