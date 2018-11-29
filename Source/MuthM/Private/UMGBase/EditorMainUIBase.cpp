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

}

void UEditorMainUIBase::PauseMusic()
{

}

void UEditorMainUIBase::Init(FMusicInfo MusicInfo, TScriptInterface<class IMMScript> MMScript)
{
	//Add Editor Panel
	OnFillMusicInfo(MusicInfo);
	_EditorPanel = Cast<UEditorPanelBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get()->GetEditorPanel(), "EditorPanel"));
	OnPlaceEditorPanel(_EditorPanel);
}
