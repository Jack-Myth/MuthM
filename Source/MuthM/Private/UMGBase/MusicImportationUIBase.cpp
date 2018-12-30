// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicImportationUIBase.h"
#include "MusicManager.h"
#include "FileHelper.h"
#include "MuthMBPLib.h"

void UMusicImportationUIBase::BeginImportMusic(const FString& Title, const FString& Musician)
{
	//XXX:Treat all music file as MP3.
	OnAsyncImportStarted();
	IMusicManager::Get(this)->ImportMP3Async(MusicFileName, Title, Musician,
		FOnMusicImportFinished::CreateLambda([this](bool IsFinished, const FMusicInfo& MusicInfo) 
			{
				if (!OnMusicImportFinished.IsBound())
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Warning:OnMusicImportFinished Unbound!");
				OnMusicImportFinished.ExecuteIfBound();
				OnAsyncImportEnd();
			}));
}