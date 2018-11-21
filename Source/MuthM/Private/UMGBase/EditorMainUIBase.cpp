// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "EditorMainUIBase.h"
#include "MuthMInEditorMode.h"
#include "Kismet/GameplayStatics.h"

void UEditorMainUIBase::UIEnterPIE()
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
}

void UEditorMainUIBase::Init(FMusicInfo MusicInfo, TScriptInterface<class IMMScript> MMScript)
{

}
