// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameInstance.h"
#include "InstructionManager.h"
#include "MuthMType.h"
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "PauseUIBase.h"
#include "UIProvider.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "UserWidget.h"
#include "Score/InGameState.h"
#include "MainSoundWave.h"
#include "MuthMBPLib.h"
#include "GameUIBase.h"
#include "GameResultUIBase.h"
#include "MuthMNativeLib.h"
#include "InEditorMode.h"
#include "fmod.hpp"
#include "PlayerInputHandler.h"

DEFINE_LOG_CATEGORY(MuthMInGameMode)

AInGameMode::AInGameMode()
{
	GameStateClass = AInGameState::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
	//NOTE: Maybe It's need to Attach to GameMode?
}

void AInGameMode::OnMusicPositionCallback(TScriptInterface<IMainSoundWave> MainSoundWave, float PlaybackPercent)
{
	float MusicLength = MainSoundWave->GetSoundDuration();
	IInstructionManager::Get(this)->Tick(PlaybackPercent*MusicLength);
	_GameTime = MusicLength * PlaybackPercent;
	OnMusicPlaybackTimeUpdate.Broadcast(_GameTime, MusicLength);
}

void AInGameMode::StartGame(FMusicInfo MusicInfo, const TArray<uint8>& MMSData)
{
	_CachedMMSData = MMSData;
	_MainMMSInstance = IInstructionManager::Get(this)->GenMMScript(false);
	_MainMMSInstance->SetPlayType(TargetPlayType);
	_MainMMSInstance->LoadFromData(MMSData);
	float SuitDelay = _MainMMSInstance->GetSuiltableDelay();
	if (!::IsValid(_GameMainMusic.GetObject()))
	{
		UE_LOG(MuthMInGameMode, Warning, TEXT("Load Main Music Failed"));
	}
	else
	{
		DelayCounter = SuitDelay;
		_MainSoundComponent->SetMainSoundWave(_GameMainMusic);
		GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, [=]()
			{
				DelayCounter -= 0.02f;
				if (DelayCounter <= 0)
				{
					_MainSoundComponent->Play();
					GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
				}
				else
					OnMusicPositionCallback(_GameMainMusic, -(DelayCounter / _GameMainMusic->GetSoundDuration()));
			}, 0.02f, true,0.02f);
	}
	//UNDONE: Debug
	TSubclassOf<UGameUIBase> GameUIClass = UUIProvider::Get(this)->GetGameUI();
	_MainGameUI = Cast<UGameUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), GameUIClass, NAME_None));
	_MainGameUI->Init(MusicInfo);
	_MainGameUI->AddToViewport(50);
	SetActorTickEnabled(true);
}

void AInGameMode::PauseGame()
{
	UGameplayStatics::SetGamePaused(this, true);
	//Prevent to construct duplicated widget.
	//And ensure the PauseUI has been generated.
	if (!::IsValid(pPauseUI))
	{
		TSubclassOf<UPauseUIBase> PauseUIClass = UUIProvider::Get(this)->GetPauseUI();
		UPauseUIBase* PauseUIInstance = Cast<UPauseUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), PauseUIClass, NAME_None));
		//It's ready for mod,Framework shouldn't load failed.
		check(PauseUIInstance)
		PauseUIInstance->AddToViewport(1000);
		pPauseUI = PauseUIInstance;
	}
	else
		pPauseUI->OnGamePaused();
}

void AInGameMode::ResumeGame()
{
	UGameplayStatics::SetGamePaused(this, false);
	if (pPauseUI->OnGameResumed())
		pPauseUI = nullptr;
}

void AInGameMode::RestartGame()
{
	StopGame();
	StartGame(_CachedMusicInfo, _CachedMMSData);
}

void AInGameMode::StopGame() 
{
	PrimaryActorTick.SetTickFunctionEnable(false);
	_MainMMSInstance->Destroy();
	_MainMMSInstance = nullptr;
	_MainGameUI->RemoveFromParent();
	_MainSoundComponent->Stop();
	if (DelayTimerHandle.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
}

void AInGameMode::NativeOnGameEnded(EGameEndReason GameEndReason)
{
	switch (GameEndReason)
	{
		case EGameEndReason::GER_GameFinished:
			ShowGameResult();
		case EGameEndReason::GER_ExitPIE:
			StopGame();
			break;
	}
	OnGameEnded.Broadcast(GameEndReason);
}

void AInGameMode::ShowGameResult()
{
	auto* InGameState = Cast<AInGameState>(UGameplayStatics::GetGameState(this));
	if (InGameState)
		InGameState->SaveScoreRecord();
	auto GameResultUIClass = UUIProvider::Get(this)->GetGameResultUI();
	auto* GameResultUI = Cast<UGameResultUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), GameResultUIClass, NAME_None));
	GameResultUI->Init();
	GameResultUI->AddToViewport(120);
}

void AInGameMode::BindDelegates()
{
	auto* InputHandler = Cast<APlayerInputHandler>(UGameplayStatics::GetPlayerPawn(this, 0));
	InputHandler->OnBackPressed.AddUObject(this, &AInGameMode::OnBackPressed);
}

void AInGameMode::OnBackPressed()
{
	if (UGameplayStatics::IsGamePaused(this)&&pPauseUI)
		ResumeGame();
	else
		PauseGame();
}

void AInGameMode::ReturnToMainMenu()
{
	NativeOnGameEnded(EGameEndReason::GER_Return);
}

void AInGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (_MainMMSInstance)
		StopGame();
}

void AInGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	//TODO:Get Property From GameInstance etc.
	FGameArgs ExchangedGameArgs = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this))->ExchangeGameArgs();
	_pMDAT = ExchangedGameArgs._MDAT;
	FMusicInfo MusicInfo = ExchangedGameArgs.MainMusicInfo;
	if (!_pMDAT->IsFileExist(ExchangedGameArgs.MMSFileName))
	{
		UE_LOG(MuthMInGameMode, Error, TEXT("Invalid MMS FileName:%d"), *ExchangedGameArgs.MMSFileName);
		//Game will freeze,Then Developer will looking for Logs.
		return;
	}
	_CachedMusicInfo = MusicInfo;
	_GameMainMusic = IMusicManager::Get(this)->LoadMainSoundByID(MusicInfo.ID);
	_MainSoundComponent = UMuthMBPLib::GenMainSWPlayer(this);
	_MainSoundComponent->SetMainSoundWave(_GameMainMusic);
	FOnPlaybackPercent MusicCallback;
	MusicCallback.BindUFunction(this, "OnMusicPositionCallback");
	_MainSoundComponent->AddOnPlaybackPercent(MusicCallback);
	_MMSFileName = ExchangedGameArgs.MMSFileName;
}

void AInGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartGame(_CachedMusicInfo, _pMDAT->GetFileData(_MMSFileName));
}
