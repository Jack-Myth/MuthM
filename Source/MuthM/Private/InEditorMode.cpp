// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InEditorMode.h"
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
#include "MainSWPlayer.h"
#include "EditorPanelBase.h"
#include "InPIEMode.h"
#include "MuthMBPLib.h"
#include "GameFramework/WorldSettings.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY(MuthMInEditorMode)

void AInEditorMode::BeginPlay()
{
	Super::BeginPlay();
	TArray<uint8> PCMData;
	_GameMainMusic->GenPCMData(PCMData);
	GetMainMMSInstance()->SetPlayType(EPlayType::PT_PIE);
	_EditorMMSInstance = IInstructionManager::Get(this)->GenMMScript(true);
	auto EditorMainUIClass = UUIProvider::Get(this)->GetEditorMainUI();
	EditorMainUI = Cast<UEditorMainUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), EditorMainUIClass, NAME_None));
	EditorMainUI->Init(GetMusicInfo(), _EditorMMSInstance);
	int BPM = MuthMNativeLib::NativeDetectBPMFromPCM(PCMData, _GameMainMusic->GetSampleRate(), _GameMainMusic->GetNumChannels());
	EditorMainUI->GetEditorPanel()->FillBPMInfo(BPM);
	EditorMainUI->AddToViewport(100);
}

bool AInEditorMode::GenPIEEnvironment(class UWorld*& PIEWorld)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	FWorldContext* WorldContext;
	GameInstance->EnterPIEMode(WorldContext);
	FURL tmpURL;
	tmpURL.Map = "PIE";
	FString err;
	GEngine->LoadMap(*WorldContext, tmpURL, nullptr, err);
	//GEngine->SetClientTravel(PIEWorld, TEXT("PIE"), ETravelType::TRAVEL_Absolute);
	//GEngine->TickWorldTravel(, 0);
	//UGameplayStatics::OpenLevel(PIEWorld, "PIE");
	PIEWorld = WorldContext->World();
	return true;
}

void AInEditorMode::EnterPIE()
{
	//TODO: Enter PIE
	if (_PIEWorld)
	{
		UE_LOG(MuthMInEditorMode, Error, TEXT("PIE World already Exist!"));
		return;
	}
	//Fill GameInstance GameArgs
	FGameArgs PIEGameArgs;
	PIEGameArgs.bIsEditorMode = true;
	PIEGameArgs.MainMusicInfo = _CachedMusicInfo;
	PIEGameArgs.MDATFilePath = _pMDAT->GetLocalFileName();
	PIEGameArgs.MMSFileName = _MMSFileName;
	PIEGameArgs._MDAT = _pMDAT;
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameInstance->FillGameArgs(PIEGameArgs);
	if (!GenPIEEnvironment(_PIEWorld))
	{
		UE_LOG(MuthMInEditorMode, Error, TEXT("Gen PIE World failed!"));
		return;
	}
	GameInstance->OnExitPIE.AddUObject(this, &AInEditorMode::ExitPIE);
	OnEnterPIE.Broadcast();
}

void AInEditorMode::ExitPIE()
{
	//TODO: Exit PIE
	StopGame();
	OnExitPIE.Broadcast();
}

void AInEditorMode::Save()
{
	TArray<uint8> MMSData = _EditorMMSInstance->SerializeToData();
	_pMDAT->RemoveFile(_MMSFileName);
	_pMDAT->AddFile(_MMSFileName, MMSData);
	_pMDAT->Save();
}

void AInEditorMode::PlayMusicOnly(float BeginTime)
{
	_MainSoundComponent->SetPaused(true);
	_MainSoundComponent->Play(BeginTime);
	SetActorTickEnabled(true);
}

void AInEditorMode::PauseMusicOnly()
{
	_MainSoundComponent->SetPaused(true);
	SetActorTickEnabled(false);
}

void AInEditorMode::NativeOnGameEnded(EGameEndReason GameEndReason)
{
	switch (GameEndReason)
	{
		case EGameEndReason::GER_GameFinished:
			ExitPIE();
			break;
		case EGameEndReason::GER_ExitPIE:
			ExitPIE();
			break;
		case EGameEndReason::GER_Return:
			ExitPIE();
			break;
	}
	OnGameEnded.Broadcast(GameEndReason);
}

TArray<class UTexture2D*> AInEditorMode::DrawMainMusicSpectrum(float BeginTime, float EndTime, uint32 ResTime, int32 ResFrequency)
{
	TArray<uint8> PCMData;
	_GameMainMusic->GenPCMData(PCMData);
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
#if !defined(PLATFORM_WINDOWS)||!PLATFORM_WINDOWS
		uint32 alignment= SpectrumTexture->PlatformData->Mips[0].BulkData.GetBulkDataAlignment();
		uint32 AlignedWidth = PartResTime + (4 - PartResTime % 4) % 4;
#endif
		for (uint32 x = 0; x < PartResTime; x++)
		{
			//X for Time
			MuthMNativeLib::NativeCalculateFrequencySpectrum(PCMData, _GameMainMusic->GetSampleRate(), _GameMainMusic->GetNumChannels(), false, PartBeginTime+x * TimeLength, TimeLength, ResFrequency, OutArray);
			for (int y = 0; y < OutArray[0].Num(); y++)
			{
				//Y for Frequency
#if !defined(PLATFORM_WINDOWS)||!PLATFORM_WINDOWS
				MipmapData[(OutArray[0].Num() - y - 1)*AlignedWidth + x] = FMath::IsFinite(OutArray[0][y]) ? FMath::Clamp<int>((OutArray[0][y] + 50) * 2, 0, 255) : 0.f;
#else
				MipmapData[(OutArray[0].Num() - y - 1)*PartResTime + x] = FMath::IsFinite(OutArray[0][y]) ? FMath::Clamp<int>((OutArray[0][y] + 50) * 2, 0, 255) : 0.f;
#endif
			}
		}
		SpectrumTexture->PlatformData->Mips[0].BulkData.Unlock();
		SpectrumTexture->UpdateResource();
		SpectrumTextures.Add(SpectrumTexture);
	}
	return SpectrumTextures;
}

void AInEditorMode::SetMusicPlaySpeed(float PlaySpeed)
{
	PlaySpeed = FMath::Clamp<float>(PlaySpeed, 0.1, 10);
	this->CustomTimeDilation = PlaySpeed;
	_MainSoundComponent->SetPaused(true);
	_MainSoundComponent->SetPitch(PlaySpeed);
}

