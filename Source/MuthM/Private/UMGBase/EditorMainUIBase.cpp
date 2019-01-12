// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "EditorMainUIBase.h"
#include "MuthMInEditorMode.h"
#include "Kismet/GameplayStatics.h"
#include "UIProvider.h"
#include "EditorPanelBase.h"

void UEditorMainUIBase::PlayMusic()
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	InEditorMode->PlayMusicOnly(GetEditorPanel()->TimeAxis);
}

void UEditorMainUIBase::PauseMusic()
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	InEditorMode->PauseMusicOnly();
}

void UEditorMainUIBase::Init(FMusicInfo MusicInfo, TScriptInterface<class IMMScript> MMScript)
{
	//Add Editor Panel
	OnFillMusicInfo(MusicInfo);
	_EditorPanel = Cast<UEditorPanelBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get(this)->GetEditorPanel(), NAME_None));
	OnPlaceEditorPanel(_EditorPanel);
}

void UEditorMainUIBase::SetPlaySpeed(float PlaySpeed)
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	InEditorMode->SetMusicPlaySpeed(PlaySpeed);
}
