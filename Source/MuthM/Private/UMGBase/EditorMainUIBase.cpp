// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "EditorMainUIBase.h"
#include "MuthMInEditorMode.h"
#include "Kismet/GameplayStatics.h"
#include "UIProvider.h"
#include "EditorPanelBase.h"

void UEditorMainUIBase::UIEnterPIE()
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	InEditorMode->EnterPIE(0);
}

void UEditorMainUIBase::UIExitPIE()
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	InEditorMode->ExitPIE();
}

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
	_EditorPanel = Cast<UEditorPanelBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get(this)->GetEditorPanel(), "EditorPanel"));
	OnPlaceEditorPanel(_EditorPanel);
}

void UEditorMainUIBase::NativeOnFillBPMInfo(float BPM)
{
	//XXX:It may be a wrong design,because it's the panel to hold instruction's alignment.
	OnFillBPMInfo(BPM);
}
