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

