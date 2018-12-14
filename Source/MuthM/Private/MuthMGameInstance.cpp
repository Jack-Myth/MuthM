// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMGameInstance.h"
#include "CoreDelegates.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameModeBase.h"
#include "ResManagers/MusicSaveGame.h"
#include "ResManagers/UserSaveGame.h"

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

void UMuthMGameInstance::GetLastScoreInfo(FString& MDATFilePath, int& ScoreIndex, bool& IsEditorMode)
{
	MDATFilePath = _GameArgs.MMSFileName;
	ScoreIndex = _GameArgs.ScoreIndex;
	IsEditorMode = _GameArgs.bIsEditorMode;
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

TWeakPtr<class UUserSaveGame> UMuthMGameInstance::GetUserSaveGame()
{
	if (UserSaveGame.IsValid())
		return UserSaveGame;
	else
	{
		TSharedPtr<UUserSaveGame> tmpUserSaveGame = MakeShareable(Cast<UUserSaveGame>(UGameplayStatics::LoadGameFromSlot("GlobalUserSaveGame", 0)));
		UserSaveGame = tmpUserSaveGame;
		return UserSaveGame;
	}
}

TWeakPtr<class UMusicSaveGame> UMuthMGameInstance::GetMusicSaveGame()
{
	if (MusicSaveGame.IsValid())
		return MusicSaveGame;
	else
	{
		TSharedPtr<UMusicSaveGame> tmpMusicSaveGame= MakeShareable(Cast<UMusicSaveGame>(UGameplayStatics::LoadGameFromSlot("GlobalMusicSaveGame", 0)));
		MusicSaveGame = tmpMusicSaveGame;
		return MusicSaveGame;
	}
}
