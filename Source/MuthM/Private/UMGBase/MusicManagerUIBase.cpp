// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicManagerUIBase.h"
#include "MuthMBPLib.h"
#include "UIProvider.h"
#include "MusicImportationUIBase.h"

#define LOCTEXT_NAMESPACE "MuthM"

TArray<FMusicInfo> UMusicManagerUIBase::GetLocalMusicInfo() const 
{
	return IMusicManager::Get(this)->GetLocalMusicList();
}

void UMusicManagerUIBase::DeleteMusic(int ID)
{
	IMusicManager::Get(this)->DeleteMusic(ID);
}

void UMusicManagerUIBase::ImportMusic(FOnMusicImported OnMusicImported)
{
	TArray<FString> Filters;
	Filters.Add("MP3");
	TArray<FString> SelectedFileName;
	if (!UMuthMBPLib::GetOpenFileName(LOCTEXT("Select Music", "Select Music"), Filters, false, SelectedFileName))
		return;
	//Currently only support import one music at once.
	auto UIClass = UUIProvider::Get(this)->GetMusicImportationUI();
	auto* UIInstance = Cast<UMusicImportationUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UIClass, "MusicImportationUI"));
	UIInstance->MusicFileName = SelectedFileName[0];
	UIInstance->InitMusicInfo(SelectedFileName[0], "", "");
	UIInstance->OnMusicImportFinished.CreateLambda([=]()
		{
			OnMusicImported.ExecuteIfBound();
		});
	UIInstance->AddToViewport(102);
}

#undef LOCTEXT_NAMESPACE