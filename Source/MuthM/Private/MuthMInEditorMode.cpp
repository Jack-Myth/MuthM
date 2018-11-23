// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMInEditorMode.h"
#include "UIProvider.h"
#include "MuthMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMType.h"
#include "MusicManager.h"
#include "InstructionManager.h"
#include "Components/AudioComponent.h"

DEFINE_LOG_CATEGORY(MuthMInEditorMode)

void AMuthMInEditorMode::BeginPlay()
{
	AMuthMGameModeBase::BeginPlay();  //Don't Call InGameMode's BeginPlay
	FGameArgs tmpGameArg = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this))->ExchangeGameArgs();
	_pMDAT = tmpGameArg._MDAT;
	MusicInfo = tmpGameArg.MainMusicInfo;
	EditorMainUI = Cast<UEditorMainUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get()->GetEditorMainUI(), "EditorMainUI"));
	TArray<uint8> MainMusicData;
	if (!IMusicManager::Get()->LoadMusicDataByID(tmpGameArg.MainMusicInfo.ID, MainMusicData))
	{
		UE_LOG(MuthMInEditorMode, Error, TEXT("Unable To Load Music!"));
		return;
	}
	IMusicManager::Get()->GenSoundWaveByOpus(MainMusicData);
	_GameMainMusic = IMusicManager::Get()->GenSoundWaveByOpus(MainMusicData);
	TArray<uint8> MMSData;
	_EditorMMSInstance = IInstructionManager::Get()->GenMMScript(true);
	if (_pMDAT->IsFileExist(tmpGameArg.MMSFileName))
	{
		MMSData = _pMDAT->GetFileData(tmpGameArg.MMSFileName);
		_EditorMMSInstance->LoadFromData(MMSData);
	}
	EditorMainUI->Init(tmpGameArg.MainMusicInfo, _EditorMMSInstance);
	EditorMainUI->AddToViewport(-1);
}

void AMuthMInEditorMode::EnterPIE(float BeginTime)
{

}

void AMuthMInEditorMode::ExitPIE()
{

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

