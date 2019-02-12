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
#include "PlayerInputHandler.h"

DEFINE_LOG_CATEGORY(MuthMInEditorMode)

void AInEditorMode::BeginPlay()
{
	AMuthMGameModeBase::BeginPlay();
	BindDelegates();
	TArray<uint8> PCMData;
	_GameMainMusic->GenPCMData(PCMData);
	_EditorMMSInstance = IInstructionManager::Get(this)->GenMMScript(true);
	_EditorMMSInstance->LoadFromData(_pMDAT->GetFileData(_MMSFileName));
	auto EditorMainUIClass = UUIProvider::Get(this)->GetEditorMainUI();
	EditorMainUI = Cast<UEditorMainUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), EditorMainUIClass, NAME_None));
	EditorMainUI->Init(GetMusicInfo());
	int BPM = MuthMNativeLib::NativeDetectBPMFromPCM(PCMData, _GameMainMusic->GetSampleRate(), _GameMainMusic->GetNumChannels());
	EditorMainUI->GetEditorPanel()->FillBPMInfo(BPM);
	EditorMainUI->AddToViewport(100);
}
void AInEditorMode::BindDelegates()
{
	auto* InputHandler = Cast<APlayerInputHandler>(UGameplayStatics::GetPlayerPawn(this, 0));
	InputHandler->OnBackPressed.AddUObject(this, &AInEditorMode::OnBackPressed);
}

void AInEditorMode::OnBackPressed()
{
	//Save and close;
}

void AInEditorMode::EnterPIE()
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance->IsInPIEMode())
	{
		UE_LOG(MuthMInEditorMode, Error, TEXT("Already in PIE Mode"));
		return;
	}
	//Fill GameInstance GameArgs
	FGameArgs PIEGameArgs;
	PIEGameArgs.bIsEditorMode = true;
	PIEGameArgs.MainMusicInfo = _CachedMusicInfo;
	PIEGameArgs.MDATFileName = _CachedMDATFileName;
	PIEGameArgs.MMSFileName = _MMSFileName;
	PIEGameArgs._MDAT = _pMDAT;
	PIEGameArgs.ScoreIndex = -1;
	GameInstance->FillGameArgs(PIEGameArgs);
	FWorldContext* WorldContext;
	GameInstance->EnterPIEMode(WorldContext);
	FURL tmpURL(TEXT("/Game/MuthM/Maps/PIE"));
	FString err;
	GEngine->Browse(*WorldContext, tmpURL, err);
	if (!(WorldContext&&WorldContext->World()))
	{
		UE_LOG(MuthMInEditorMode, Error, TEXT("Gen PIE World failed!"));
		return;
	}
	EditorMainUI->RemoveFromParent();
	GameInstance->OnExitPIE.AddUObject(this, &AInEditorMode::NativeOnExitPIE);
	OnEnterPIE.Broadcast();
}

void AInEditorMode::NativeOnExitPIE()
{
	//TODO: Exit PIE
	EditorMainUI->AddToViewport(100);
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
		int32 AlignedWidth = PartResTime;
		//if (ResTime % Alignment)
			//AlignedWidth += (4 - PartResTime % 4);
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

void AInEditorMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	_EditorMMSInstance->Destroy();
}

