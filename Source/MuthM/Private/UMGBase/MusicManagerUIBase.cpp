// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicManagerUIBase.h"
#include "MuthMBPLib.h"

#define LOCTEXT_NAMESPACE "MuthM"

TArray<FMusicInfo> UMusicManagerUIBase::GetLocalMusicInfo()
{
	return IMusicManager::Get(this)->GetLocalMusicList();
}

void UMusicManagerUIBase::DeleteMusic(int ID)
{
	IMusicManager::Get(this)->DeleteMusic(ID);
}

bool UMusicManagerUIBase::ImportMusic()
{
	TArray<FString> Filters;
	Filters.Add("MP3");
	TArray<FString> SelectedFileName;
	if (!UMuthMBPLib::GetOpenFileName(LOCTEXT("Select Music", "Select Music"), Filters, false, SelectedFileName))
		return false;
	//Currently only support import one music at once.
	return false;
}

#undef LOCTEXT_NAMESPACE