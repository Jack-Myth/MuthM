// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMInEditorMode.h"
#include "UIProvider.h"
#include "MuthMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMType.h"
#include "MusicManager.h"
#include "InstructionManager.h"
#include "Components/AudioComponent.h"
#include "MainSoundWave.h"
#include "EditorMainUIBase.h"
#include "MuthMNativeLib.h"
#include "Engine/Texture2D.h"

DEFINE_LOG_CATEGORY(MuthMInEditorMode)

void AMuthMInEditorMode::BeginPlay()
{
	Super::BeginPlay();
	TArray<uint8> PCMData;
	_GameMainMusic->DecodePCMFromCompressedData(PCMData);
	GetMainMMSInstance()->SetPlayType(EPlayType::PT_PIE);
	_EditorMMSInstance = IInstructionManager::Get(this)->GenMMScript(true);
	auto EditorMainUIClass = UUIProvider::Get(this)->GetEditorMainUI();
	EditorMainUI = Cast<UEditorMainUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), EditorMainUIClass, "EditorMainUI"));
	EditorMainUI->Init(GetMusicInfo(), _EditorMMSInstance);
	int BPM = MuthMNativeLib::NativeDetectBPMFromPCM(PCMData, _GameMainMusic->GetSampleRate(), _GameMainMusic->NumChannels);
	EditorMainUI->NativeOnFillBPMInfo(BPM);
	EditorMainUI->AddToViewport(100);
}

void AMuthMInEditorMode::EnterPIE(float BeginTime)
{
	//TODO: Enter PIE
	StartGame(GetMusicInfo(), _EditorMMSInstance->Serialize());
	OnEnterPIE.Broadcast();
}

void AMuthMInEditorMode::ExitPIE()
{
	//TODO: Exit PIE
	StopGame();
	OnExitPIE.Broadcast();
}

void AMuthMInEditorMode::PlayMusicOnly(float BeginTime)
{
	_MainSoundComponent->SetPaused(false);
	_MainSoundComponent->Play(BeginTime);
}

void AMuthMInEditorMode::PauseMusicOnly()
{
	_MainSoundComponent->SetPaused(true);
}

void AMuthMInEditorMode::NativeOnGameEnded(FGameEndReason GameEndReason)
{
	switch (GameEndReason)
	{
		case FGameEndReason::GER_GameFinished:
			ExitPIE();
			break;
		case FGameEndReason::GER_ExitPIE:
			ExitPIE();
			break;
		case FGameEndReason::GER_Return:
			ExitPIE();
			break;
	}
	OnGameEnded.Broadcast(GameEndReason);
}
void CalculateFrequencySpectrum(TArray<uint8>& PCMData, int Channels, const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 SpectrumWidth, TArray< TArray<float> >& OutSpectrums);
class UTexture2D* AMuthMInEditorMode::DrawMainMusicSpectrum(float BeginTime, float EndTime, uint32 ResTime, int32 ResFrequency)
{
	float TimeLength = (EndTime - BeginTime) / ResTime;
	TArray<TArray<float>> OutArray;
	UTexture2D* SpectrumTexture = UTexture2D::CreateTransient(ResTime, ResFrequency, EPixelFormat::PF_A8);
	TArray<uint8> PCMData;
	_GameMainMusic->DecodePCMFromCompressedData(PCMData);
	uint8* MipmapData = (uint8*)SpectrumTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	for (uint32 x = 0; x < ResTime; x++)
	{
		//X for Time
		CalculateFrequencySpectrum(PCMData, _GameMainMusic->NumChannels, false, BeginTime + x * TimeLength, TimeLength, ResFrequency, OutArray);
		//MuthMNativeLib::NativeCalculateFrequencySpectrum(PCMData, _GameMainMusic->GetSampleRate(), _GameMainMusic->NumChannels, false, BeginTime + x * TimeLength, TimeLength, ResFrequency, OutArray);
		for (int y = 0; y < OutArray[0].Num(); y++)
		{
			//Y for Frequency
			MipmapData[(OutArray[0].Num()-y-1)*ResTime + x] = FMath::Clamp<int>((OutArray[0][y]+50)*2, 0, 255);
		}
	}
	SpectrumTexture->PlatformData->Mips[0].BulkData.Unlock();
	SpectrumTexture->UpdateResource();
	return SpectrumTexture;
}

