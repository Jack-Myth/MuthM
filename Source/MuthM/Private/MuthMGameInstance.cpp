// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMGameInstance.h"
#include "CoreDelegates.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameModeBase.h"
#include "ResManagers/GlobalSaveGame.h"
#include "InstructionManager.h"
#include "RhythmTap.h"
#include "LoadScene.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY(MuthMGameInstance);

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
	if (!GetGlobalSaveGame())
	{
		TSharedPtr<UGlobalSaveGame> tmpSaveGame = MakeShareable(NewObject<UGlobalSaveGame>());
		UGameplayStatics::SaveGameToSlot(tmpSaveGame.Get(), "GlobalSaveGame", 0);
		GlobalSaveGame = tmpSaveGame;
	}
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

TSharedPtr<class UGlobalSaveGame> UMuthMGameInstance::GetGlobalSaveGame()
{
	if (GlobalSaveGame.IsValid())
		return GlobalSaveGame.Pin();
	else
	{
		TSharedPtr<UGlobalSaveGame> tmpUserSaveGame = MakeShareable(Cast<UGlobalSaveGame>(UGameplayStatics::LoadGameFromSlot("GlobalSaveGame", 0)));
		GlobalSaveGame = tmpUserSaveGame;
		return GlobalSaveGame.Pin();
	}
}

void UMuthMGameInstance::SaveGlobalSaveGame()
{
	if (GlobalSaveGame.IsValid())
		UGameplayStatics::SaveGameToSlot(GlobalSaveGame.Pin().Get(), "GlobalSaveGame", 0);
}

void UMuthMGameInstance::Init()
{
	//Initialize the whole game.
	//Cache GameWorldContext
	GameWorldContext = WorldContext;

	//Register Instructions
	auto InstructionManager = IInstructionManager::Get(this);
	TMap<FName, TSubclassOf<UInstruction>> InstructionClasses;
	//Fill Classes.
	InstructionClasses.Add("LoadScene") = ULoadScene::StaticClass();
	InstructionClasses.Add("RhythmTap") = URhythmTap::StaticClass();
	CachedInstructionRef.SetNum(InstructionClasses.Num());
	int InstructionRefIndex = 0;
	for (auto it=InstructionClasses.CreateIterator();it;++it)
		InstructionManager->RegisterInstruction(it->Key, it->Value, CachedInstructionRef[InstructionRefIndex++]);
}

void UMuthMGameInstance::EnterPIEMode(class UWorld* PIEWorld)
{
	if (WorldContext==PIEWorldContext)
	{
		UE_LOG(MuthMGameInstance, Error, TEXT("GameInstance already in PIE Mode!"));
		return;
	}
	PIEWorldContext = GEngine->GetWorldContextFromWorld(PIEWorld);
	if (!PIEWorldContext)
	{
		//Create new WorldContext
		FWorldContext& newWorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
		newWorldContext.SetCurrentWorld(PIEWorld);
		PIEWorldContext = GEngine->GetWorldContextFromWorld(PIEWorld);
	}
	WorldContext = PIEWorldContext;
}

void UMuthMGameInstance::ExitPIEMode()
{
	WorldContext = GameWorldContext;
	PIEWorldContext = nullptr;
}
