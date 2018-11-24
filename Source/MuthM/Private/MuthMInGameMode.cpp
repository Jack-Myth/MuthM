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
#include "Classes/SoundVisualizationStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "UserWidget.h"
#include "Score/ScoreCore.h"

DEFINE_LOG_CATEGORY(MuthMInGameMode);

AMuthMInGameMode::AMuthMInGameMode()
{
	_MainSoundComponent = CreateDefaultSubobject<UAudioComponent>("_MainAudioComponent");
	_ScoreCore = CreateDefaultSubobject<UScoreCore>("_ScoreCore");
	//NOTE: Maybe It's need to Attach to GameMode?
}

class UScoreCore* AMuthMInGameMode::GetScoreCore()
{
	return _ScoreCore;
}

void AMuthMInGameMode::Tick(float DeltaSeconds)
{
	MusicPlaybackTime += DeltaSeconds;
}

void AMuthMInGameMode::StartGame(FMusicInfo MusicInfo, const TArray<uint8>& MMSData)
{
	_MainMMSInstance = IInstructionManager::Get()->GenMMScript(false);
	_MainMMSInstance->LoadFromData(MMSData);
	float SuitDelay = _MainMMSInstance->GetSuiltableDelay();
	TArray<uint8> OpusData;
	IMusicManager::Get()->LoadMusicDataByID(MusicInfo.ID, OpusData);
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
	TSubclassOf<UGameUIBase> GameUIClass = UUIProvider::Get()->GetGameUI();
	_MainGameUI = Cast<UGameUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), GameUIClass, "GameUI"));
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

void AMuthMInGameMode::StopGame()
{
	_MainMMSInstance->Destroy();
	_MainMMSInstance = nullptr;
}

void AMuthMInGameMode::DrawMainMusicSpectrum(class UTextureRenderTarget2D* RenderTarget2D, float BeginTime, float EndTime, uint32 ResTime, int32 ResFrequency)
{
	float TimeLength = (EndTime - BeginTime) / ResTime;
	TArray<float> OutArray;
	UCanvas* SpectrumCanvas;
	FCanvasBoxItem tmpBoxItem(FVector2D(0,0),FVector2D(RenderTarget2D->SizeX/ResTime, RenderTarget2D->SizeY / ResFrequency)); 
	FDrawToRenderTargetContext DrawRenderTargetContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RenderTarget2D, SpectrumCanvas, FVector2D(RenderTarget2D->SizeX, RenderTarget2D->SizeY), DrawRenderTargetContext);
	for (int x=0;x<ResTime;x++)
	{
		//X for Time
		USoundVisualizationStatics::CalculateFrequencySpectrum(_GameMainMusic, 0, BeginTime + x * TimeLength, TimeLength, ResFrequency, OutArray);
		for (int y=0;y<OutArray.Num();y++)
		{
			//Y for Frequency
			tmpBoxItem.SetColor(FLinearColor(OutArray[y], OutArray[y], OutArray[y]));
			SpectrumCanvas->DrawItem(tmpBoxItem, x*tmpBoxItem.Size.X, y*tmpBoxItem.Size.Y);
		}
	}
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, DrawRenderTargetContext);
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
	StartGame(MusicInfo, _pMDAT->GetFileData(ExchangedGameArgs.MMSFileName));
}
