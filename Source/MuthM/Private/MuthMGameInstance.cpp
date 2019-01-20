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
#include "Engine/GameViewportClient.h"
#include "SlateApplication.h"
#include "SceneViewport.h"
#include "SViewport.h"
#include "SGameLayerManager.h"
#include "SWindow.h"

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

void UMuthMGameInstance::EnterPIEMode(struct FWorldContext*& PIEWorldContext)
{
	if (PIESession.IsValid())
	{
		UE_LOG(MuthMGameInstance, Error, TEXT("GameInstance already in PIE Mode!"));
		return;
	}
	PIESession = MakeShareable(new FMuthMPIEInfo());
	PIESession->GameWorldContext = WorldContext;
	TSharedPtr<SWindow> GameWindow = PIESession->GameWorldContext->GameViewport->GetWindow();
	//Create new WorldContext
	FWorldContext& newWorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	PIEWorldContext = &newWorldContext;
	PIESession->PIEWorldContext = &newWorldContext;
	PIESession->PIEWorldContext->OwningGameInstance = this;
	PIESession->PIEWorldContext->GameViewport = NewObject<UGameViewportClient>(GEngine, GEngine->GameViewportClientClass);
	PIESession->PIEWorldContext->GameViewport->Init(*(PIESession->PIEWorldContext), this);
	PIESession->GameViewportWidget = PIESession->GameWorldContext->GameViewport->GetGameViewportWidget();
	PIESession->PIEViewport = MakeShareable(new FSceneViewport(PIESession->PIEWorldContext->GameViewport, PIESession->GameViewportWidget));
	FString error;
	PIESession->GameViewport = StaticCastSharedPtr<FSceneViewport>(PIESession->GameViewportWidget->GetViewportInterface().Pin());
	PIESession->PIEViewport->SetViewportClient(PIESession->PIEWorldContext->GameViewport);
	PIESession->PIEWorldContext->GameViewport->SetViewport(PIESession->PIEViewport.Get());
	PIESession->GameViewportWidget->SetViewportInterface(PIESession->PIEViewport.ToSharedRef());
	//GEngine->GameViewport = PIESession->PIEWorldContext->GameViewport;
	//PIESession->GameViewportClientRef = PIESession->GameWorldContext->GameViewport;
	WorldContext = &newWorldContext;
	OnEnterPIE.Broadcast();
	//PIESession->PIEWorldContext->GameViewport->SetupInitialLocalPlayer(error);
	//Resize frame, all paramter use the GameViewport.
	//PIESession->PIEViewport->ResizeFrame(GameViewport->GetSize().X, GameViewport->GetSize().Y,GameViewport->GetWindowMode());
	//UGameViewportClient::OnViewportCreated().Broadcast();
	//FSlateApplication::Get().RegisterGameViewport(PIESession->PIEViewportWidget.ToSharedRef());
	//PIESession->PIEViewport->UpdateViewportRHI(false, GameViewport->GetSize().X, GameViewport->GetSize().Y, GameViewport->GetWindowMode(), EPixelFormat::PF_A8R8G8B8);
	//PIESession->GameWorldContext->GameViewport->GetGameViewport()->OnPlayWorldViewportSwapped(PIESession->PIEViewport.ToSharedRef().Get());
}

void UMuthMGameInstance::ExitPIEMode()
{
	//Restore GameViewport.
	GEngine->DestroyWorldContext(GetWorld());
	PIESession->GameViewportWidget->SetViewportInterface(PIESession->GameViewport.ToSharedRef());
	PIESession->GameWorldContext->GameViewport = PIESession->GameViewportClientRef;
	WorldContext = PIESession->GameWorldContext;
	//Release PIESession
	PIESession.Reset();
}
