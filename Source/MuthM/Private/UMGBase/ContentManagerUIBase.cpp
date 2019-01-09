// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ContentManagerUIBase.h"
#include "UIProvider.h"
#include "MusicManagerUIBase.h"
#include "ScoreManagerUIBase.h"
#include "ModManagerUIBase.h"

class UMusicManagerUIBase* UContentManagerUIBase::GetMusicManagerUI()
{
	if (!MusicManagerUI)
	{
		TSubclassOf<UMusicManagerUIBase> UIClass = UUIProvider::Get(this)->GetMusicManagerUI();
		MusicManagerUI = Cast<UMusicManagerUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UIClass, NAME_None));
		MusicManagerUI->OnInitMusicList();
	}
	return MusicManagerUI;
}

class UScoreManagerUIBase* UContentManagerUIBase::GetScoreManagerUI()
{
	if (!ScoreManagerUI)
	{
		TSubclassOf<UScoreManagerUIBase> UIClass = UUIProvider::Get(this)->GetScoreManagerUI();
		ScoreManagerUI = Cast<UScoreManagerUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UIClass, NAME_None));
	}
	return ScoreManagerUI;
}

class UModManagerUIBase* UContentManagerUIBase::GetModManagerUI()
{
	if (!ModManagerUI)
	{
		TSubclassOf<UModManagerUIBase> UIClass = UUIProvider::Get(this)->GetModManagerUI();
		ModManagerUI = Cast<UModManagerUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UIClass, NAME_None));
	}
	return ModManagerUI;
}
