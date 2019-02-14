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
#include "MuthMNativeLib.h"
#include "fmod.hpp"
#include "Engine/LocalPlayer.h"
#include "UserWidget.h"
#include "UObjectIterator.h"
#include "UnrealEngine.h"
#include "AudioDevice.h"
#include "EngineUtils.h"
#include "Engine/LevelStreaming.h"
#include "Async.h"
#include "RhythmSlide.h"
#include "Background.h"
#include "GlobalVariable.h"

DEFINE_LOG_CATEGORY(MuthMGameInstance);

UMuthMGameInstance::UMuthMGameInstance()
{
	
}

void UMuthMGameInstance::FillGameArgs(FGameArgs targetGameArgs)
{
	int tmpScoreIndex = _GameArgs.ScoreIndex;
	_GameArgs = targetGameArgs;
	if (_GameArgs.ScoreIndex < 0)
		_GameArgs.ScoreIndex = tmpScoreIndex;
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
	InstructionClasses.Add("RhythmSlide") = URhythmSlide::StaticClass();
	InstructionClasses.Add("Background") = UBackground::StaticClass();
	InstructionClasses.Add("GlobalVariable") = UGlobalVariable::StaticClass();
	CachedInstructionRef.SetNum(InstructionClasses.Num());
	int InstructionRefIndex = 0;
	for (auto it=InstructionClasses.CreateIterator();it;++it)
		InstructionManager->RegisterInstruction(it->Key, it->Value, CachedInstructionRef[InstructionRefIndex++]);
#if defined(_MUTHM_USE_FMOD)&&_MUTHM_USE_FMOD
	//Init FModSystem
	//MuthMNativeLib::GetFModSystem()->setOutput(FMOD_OUTPUTTYPE_AUDIOTRACK);  //Crash on PC
#endif
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
	//Create PIE GameViewportClient
	PIESession->PIEWorldContext->GameViewport = NewObject<UGameViewportClient>(GEngine, GEngine->GameViewportClientClass);
	PIESession->PIEWorldContext->GameViewport->Init(*(PIESession->PIEWorldContext), this, true);
	//For GetViewportOverlayWidget() , Need to modify Engine Code to get the SharedPtr.
	PIESession->PIEWorldContext->GameViewport->SetViewportOverlayWidget(PIESession->GameWorldContext->GameViewport->GetWindow(),PIESession->GameWorldContext->GameViewport->GetViewportOverlayWidget().ToSharedRef());
	PIESession->GameViewportWidget = PIESession->GameWorldContext->GameViewport->GetGameViewportWidget();
	//Replace GameViewportClient with PIE GameViewportClient
	PIESession->GameViewport = StaticCastSharedPtr<FSceneViewport>(PIESession->GameViewportWidget->GetViewportInterface().Pin());
	PIESession->PIEWorldContext->GameViewport->SetViewport(PIESession->GameViewport.Get());
	PIESession->GameViewport->SetViewportClient(PIESession->PIEWorldContext->GameViewport);
	WorldContext = &newWorldContext;
	OnEnterPIE.Broadcast();
}

void UMuthMGameInstance::ExitPIEMode()
{
	if (!PIESession.IsValid())
	{
		UE_LOG(MuthMGameInstance, Error, TEXT("Try ExitPIEMode but the PIESession is not valid! Please ensure you are in PIE Mode."));
		return;
	}
	auto* PIEWorld = PIESession->PIEWorldContext->World();
	//Clean PIE World
	//Clean Widgets
	//CleanupGameViewport();
	for (TObjectIterator<UWorld> WorldIt; WorldIt; ++WorldIt)
	{
		//Clean all Game World Actors except "Main GameWorld"
		//It's work for StreamingLevel
		//Every StreamingLevel will create a world and will cause crash when Travel because it have no context.
		if (WorldIt->WorldType == EWorldType::Game&&*WorldIt != PIESession->GameWorldContext->World())
		{
			for (TObjectIterator<UUserWidget> it; it; ++it)
			{
				if (it->GetWorld() == *WorldIt)
					it->RemoveFromViewport();
			}
			//Destroy Players
			if (GEngine->GetWorldContextFromWorld(*WorldIt))
			{
				//Only PIE World will have World Context,So check it to prevent Streaming world execute into the loop.
				for (FLocalPlayerIterator It(GEngine, *WorldIt); It; ++It)
				{
					if (It->PlayerController)
					{
						if (It->PlayerController->GetPawn())
						{
							WorldIt->DestroyActor(It->PlayerController->GetPawn(), true);
						}
						WorldIt->DestroyActor(It->PlayerController, true);
						It->PlayerController = NULL;
					}
				}
			}
			//Stop loading SteamingLevel
			PIEWorld->bIsLevelStreamingFrozen = false;
			PIEWorld->SetShouldForceUnloadStreamingLevels(true);
			PIEWorld->FlushLevelStreaming();
			for (FActorIterator ActorIt(*WorldIt); ActorIt; ++ActorIt)
			{
				ActorIt->RouteEndPlay(EEndPlayReason::EndPlayInEditor);
			}
			WorldIt->DestroyWorld(true);
		}
	}
	//Restore GameViewportClient
	//Since I can't set the WakePtr to nullptr, Construct an empty SOverlay to replace it.
	PIESession->PIEWorldContext->GameViewport->SetViewportOverlayWidget(nullptr, SNew(SOverlay));
	//Prevent AudioDevice to be shutdown when PIEViewport Destroyed.
	//Need to hack Engine code.
	if (!GIsEditor)
		PIESession->PIEWorldContext->GameViewport->SetAudioDeviceHandle(-1);
	PIESession->GameViewport->SetViewportClient(PIESession->GameWorldContext->GameViewport);
	//Restore WorldContext
	WorldContext = PIESession->GameWorldContext;
	//It only have one Context need to destroy although there may have multiple worlds.
	GEngine->DestroyWorldContext(PIEWorld);
	//Release PIESession
	PIESession.Reset();
	//Force GC
	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	//Call Delegate
	OnExitPIE.Broadcast();
}

void UMuthMGameInstance::Shutdown()
{
	if (PIESession.IsValid())
	{
		ExitPIEMode();
	}
}

void UMuthMGameInstance::Tick(float DeltaTime)
{
	//Tick FMod Update
#if defined(_MUTHM_USE_FMOD)&&_MUTHM_USE_FMOD
	MuthMNativeLib::GetFModSystem()->update();
#endif
}

TStatId UMuthMGameInstance::GetStatId() const
{
	return TStatId();
}
