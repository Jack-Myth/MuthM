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
	Super::BeginPlay();
	//EditorMainUI->Init(, _EditorMMSInstance);
	//EditorMainUI->AddToViewport(-1);
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

