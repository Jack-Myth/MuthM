// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMMainMenuMode.h"
#include "UIProvider.h"

void AMuthMMainMenuMode::LoadWelcome()
{
	auto LogoAnimationClass = UUIProvider::Get()->GetWelcomeUI();
	auto* LogoAnimation = UUserWidget::CreateWidgetInstance(*GetWorld(), LogoAnimationClass, "LogoAnimation");
	LogoAnimation->AddToViewport();
}

void AMuthMMainMenuMode::BeginPlay()
{
	
}
