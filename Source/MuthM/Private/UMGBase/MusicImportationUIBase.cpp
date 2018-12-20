// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicImportationUIBase.h"
#include "MusicManager.h"

void UMusicImportationUIBase::BeginImportMusic(const FString& Title, const FString& Musician)
{
	//XXX:Treat all music file as MP3.
	IMusicManager::Get(this)->ImportMP3(MusicFileName, Title, Musician);
	OnMusicImportFinished.ExecuteIfBound();
}
