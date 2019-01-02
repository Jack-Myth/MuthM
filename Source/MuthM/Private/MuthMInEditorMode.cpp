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
	SetMusicPlaySpeed(1); //Reset Music play speed
	StartGame(GetMusicInfo(), _EditorMMSInstance->SerializeToData());
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
	MusicPlaybackTime = BeginTime;
	_MainSoundComponent->Play(BeginTime);
	SetActorTickEnabled(true);
}

void AMuthMInEditorMode::PauseMusicOnly()
{
	_MainSoundComponent->SetPaused(true);
	SetActorTickEnabled(false);
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

TArray<class UTexture2D*> AMuthMInEditorMode::DrawMainMusicSpectrum(float BeginTime, float EndTime, uint32 ResTime, int32 ResFrequency)
{
	TArray<uint8> PCMData;
	_GameMainMusic->DecodePCMFromCompressedData(PCMData);
	TArray<UTexture2D*> SpectrumTextures;
	int SplitedCount = (EndTime - BeginTime) / 20 + 1;
	for (int i = 0; i < SplitedCount; i++)
	{
		float PartBeginTime = FMath::Lerp(BeginTime, EndTime, (float)i / SplitedCount);
		float PartEndTime = FMath::Lerp(BeginTime, EndTime, (float)(i+1) / SplitedCount);
		uint32 PartResTime = ResTime / SplitedCount;
		float TimeLength = (PartEndTime - PartBeginTime) / PartResTime;
		TArray<TArray<float>> OutArray;
		UTexture2D* SpectrumTexture = UTexture2D::CreateTransient(PartResTime, ResFrequency, EPixelFormat::PF_A8);
		uint8* MipmapData = (uint8*)SpectrumTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		for (uint32 x = 0; x < PartResTime; x++)
		{
			//X for Time
			MuthMNativeLib::NativeCalculateFrequencySpectrum(PCMData, _GameMainMusic->GetSampleRate(), _GameMainMusic->NumChannels, false, PartBeginTime+x * TimeLength, TimeLength, ResFrequency, OutArray);
			for (int y = 0; y < OutArray[0].Num(); y++)
			{
				//Y for Frequency
				MipmapData[(OutArray[0].Num() - y - 1)*PartResTime + x] = FMath::Clamp<int>((OutArray[0][y]+50)*2, 0, 255);
			}
		}
		SpectrumTexture->PlatformData->Mips[0].BulkData.Unlock();
		SpectrumTexture->UpdateResource();
		SpectrumTextures.Add(SpectrumTexture);
	}
	return SpectrumTextures;
}

void AMuthMInEditorMode::SetMusicPlaySpeed(float PlaySpeed)
{
	PlaySpeed = FMath::Clamp<float>(PlaySpeed, 0.1, 10);
	this->CustomTimeDilation = PlaySpeed;
	_MainSoundComponent->SetPitchMultiplier(PlaySpeed);
}

