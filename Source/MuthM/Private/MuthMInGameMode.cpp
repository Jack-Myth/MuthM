// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMInGameMode.h"
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
#include "Score/ScoreCore.h"
#include "MainSoundWave.h"
#include "MuthMBPLib.h"
#include "GameUIBase.h"
#include "GameResultUIBase.h"
#include "MuthMNativeLib.h"
#include "MuthMInEditorMode.h"
#include "fmod.hpp"

DEFINE_LOG_CATEGORY(MuthMInGameMode)

AMuthMInGameMode::AMuthMInGameMode()
{
	_MainSoundComponent = CreateDefaultSubobject<UAudioComponent>("_MainAudioComponent");
	_ScoreCore = CreateDefaultSubobject<UScoreCore>("_ScoreCore");
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	//NOTE: Maybe It's need to Attach to GameMode?
}

void AMuthMInGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	MusicPlaybackTime += DeltaSeconds;
	//UNDONE:
	OnMusicPlaybackTimeUpdate.Broadcast(MusicPlaybackTime,_GameMainMusic->GetDuration());
}

void AMuthMInGameMode::StartGame(FMusicInfo MusicInfo, const TArray<uint8>& MMSData, float BeginTime)
{
	_CachedMMSData = MMSData;
	_MainMMSInstance->LoadFromData(MMSData);
	float SuitDelay = _MainMMSInstance->GetSuiltableDelay();
	MusicPlaybackTime = -SuitDelay;
	if (!::IsValid(_GameMainMusic))
	{
		UE_LOG(MuthMInGameMode, Warning, TEXT("Load Main Music Failed"));
	}
	else
	{
		_MainSoundComponent->SetSound(_GameMainMusic);
		FTimerHandle tmpTimeHandle; //No Need this TimeHandle anymore.
		GetWorld()->GetTimerManager().SetTimer(tmpTimeHandle, [=]()
			{
				_MainSoundComponent->Play();
			}, 0, false, SuitDelay);
	}
	//UNDONE: Debug
	//TSubclassOf<UGameUIBase> GameUIClass = UUIProvider::Get(this)->GetGameUI();
	//_MainGameUI = Cast<UGameUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), GameUIClass, "GameUI"));
	//_MainGameUI->Init(MusicInfo);
	//_MainGameUI->AddToViewport(50);
	SetActorTickEnabled(true);
}

void AMuthMInGameMode::PauseGame()
{
	SetActorTickEnabled(false);
	UGameplayStatics::SetGamePaused(this, true);
	//Prevent to construct duplicated widget.
	//And ensure the PauseUI has been generated.
	if (!::IsValid(pPauseUI))
	{
		TSubclassOf<UPauseUIBase> PauseUIClass = UUIProvider::Get(this)->GetPauseUI();
		UPauseUIBase* PauseUIInstance = Cast<UPauseUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), PauseUIClass, "PauseUI"));
		//It's ready for mod,Framework shouldn't load failed.
		check(PauseUIInstance)
		PauseUIInstance->AddToViewport(1000);
		pPauseUI = PauseUIInstance;
	}
	else
		pPauseUI->OnGamePaused();
}

void AMuthMInGameMode::ResumeGame()
{
	SetActorTickEnabled(true);
	UGameplayStatics::SetGamePaused(this, false);
	if (pPauseUI->OnGameResumed())
		pPauseUI = nullptr;
}

void AMuthMInGameMode::RestartGame()
{
	StopGame();
	StartGame(_CachedMusicInfo, _CachedMMSData);
}

void AMuthMInGameMode::StopGame()
{
	PrimaryActorTick.SetTickFunctionEnable(false);
	_MainMMSInstance->Destroy();
	_MainMMSInstance = nullptr;
	_MainGameUI->RemoveFromParent();
}

void AMuthMInGameMode::NativeOnGameEnded(FGameEndReason GameEndReason)
{
	switch (GameEndReason)
	{
		case FGameEndReason::GER_GameFinished:
			{
				StopGame();
				ShowGameResult();
			}
			break;
	}
	OnGameEnded.Broadcast(GameEndReason);
}

void AMuthMInGameMode::ShowGameResult()
{
	GetScoreCore()->SaveScoreRecord();
	auto GameResultUIClass = UUIProvider::Get(this)->GetGameResultUI();
	auto* GameResultUI = Cast<UGameResultUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), GameResultUIClass, "GameResultUI"));
	GameResultUI->Init();
	GameResultUI->AddToViewport(120);
}

void AMuthMInGameMode::ReturnToMainMenu()
{
	//UNDONE: Return To Main Menu()
	//The Level switch is still a problem
}

void AMuthMInGameMode::BeginPlay()
{
	Super::BeginPlay();
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
	TArray<uint8> OGGData;
	IMusicManager::Get(this)->LoadMusicDataByID(MusicInfo.ID, OGGData);
	_GameMainMusic = FMOD::System::createSound()
	_MainSoundComponent->SetSound(_GameMainMusic);
	_MainMMSInstance = IInstructionManager::Get(this)->GenMMScript(false);
	if (!this->IsA<AMuthMInEditorMode>())
		StartGame(MusicInfo, _pMDAT->GetFileData(ExchangedGameArgs.MMSFileName));
}
