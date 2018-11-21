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

DEFINE_LOG_CATEGORY(MuthMInGameMode);

AMuthMInGameMode::AMuthMInGameMode()
{
	_MainSoundComponent = CreateDefaultSubobject<UAudioComponent>("_MainAudioComponent");
	//NOTE: Maybe It's need to Attach to GameMode?
}

void AMuthMInGameMode::Tick(float DeltaSeconds)
{
	MusicPlaybackTime += DeltaSeconds;
}

void AMuthMInGameMode::PauseGame()
{
	UGameplayStatics::SetGamePaused(this, true);
	//Prevent to construct duplicated widget.
	//And ensure the PauseUI has been generated.
	if (!::IsValid(pPauseUI))
	{
		TSubclassOf<UPauseUIBase> PauseUIClass = UUIProvider::Get()->GetPauseUI();
		UPauseUIBase* PauseUIInstance = Cast<UPauseUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), PauseUIClass, "PauseUI"));
		//TODO:It's ready for mod,Framework shouldn't load failed.
		if (!PauseUIInstance)
			return;
		PauseUIInstance->AddToViewport(1000);
		pPauseUI = PauseUIInstance;
	}
	else
		pPauseUI->OnGamePaused();
}

void AMuthMInGameMode::ResumeGame()
{
	UGameplayStatics::SetGamePaused(this, false);
	if (pPauseUI->OnGameResumed())
		pPauseUI = nullptr;
}

void AMuthMInGameMode::RestartGame()
{
	//UNDONE: RestartGame
}

void AMuthMInGameMode::BeginPlay()
{
	Super::BeginPlay();
	//TODO:Get Property From GameInstance etc.
	FGameArgs ExchangedGameArgs = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this))->ExchangeGameArgs();
	_pMDAT = ExchangedGameArgs._MDAT;
	MusicInfo = ExchangedGameArgs.MainMusicInfo;
	if (!_pMDAT->IsFileExist(ExchangedGameArgs.MMSFileName))
	{
		UE_LOG(MuthMInGameMode, Error, TEXT("Invalid MMS FileName:%d"), *ExchangedGameArgs.MMSFileName);
		//Game will freeze,Then Developer will looking for Logs.
		return;
	}
	_MainMMSInstance = IInstructionManager::Get()->GenMMScript(false);
	_MainMMSInstance->LoadFromData(_pMDAT->GetFileData(ExchangedGameArgs.MMSFileName));
	float SuitDelay = _MainMMSInstance->GetSuiltableDelay();
	TArray<uint8> OpusData;
	IMusicManager::Get()->LoadMusicDataByID(ExchangedGameArgs.MainMusicInfo.ID, OpusData);
	_GameMainMusic = IMusicManager::Get()->GenSoundWaveByOpus(OpusData);
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
}
