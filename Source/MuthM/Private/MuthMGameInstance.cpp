// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMGameInstance.h"
#include "CoreDelegates.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameModeBase.h"

UMuthMGameInstance::UMuthMGameInstance()
{
	
}

void UMuthMGameInstance::FillGameArgs(FGameArgs targetGameArgs)
{
	_GameArgs = targetGameArgs;
}

FGameArgs UMuthMGameInstance::ExchangeGameArgs()
{
	FGameArgs tmpGameArgs = _GameArgs;
	_GameArgs._MDAT.Reset();
	return tmpGameArgs;
}

void UMuthMGameInstance::PostInitProperties()
{
	Super::PostInitProperties();
	FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &UMuthMGameInstance::OnApplicationDeactive);
	FCoreDelegates::ApplicationHasReactivatedDelegate.AddUObject(this, &UMuthMGameInstance::OnApplicationReactive);
	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &UMuthMGameInstance::OnApplicationSwitchBackground);
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UMuthMGameInstance::OnApplicationSwitchForeground);
}

void UMuthMGameInstance::OnApplicationDeactive()
{
	auto* GameMode=Cast<AMuthMGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
		GameMode->PauseGame();
	//XXX:It may limit the Event handle for Blueprint.
}

void UMuthMGameInstance::OnApplicationReactive()
{
	//Doesn't need to resume game.
	//XXX:Still,Those delegate handler need a better implementation.
}

void UMuthMGameInstance::OnApplicationSwitchBackground()
{
	auto* GameMode = Cast<AMuthMGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
		GameMode->PauseGame();
}

void UMuthMGameInstance::OnApplicationSwitchForeground()
{
}
