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

class USoundWave* UMusicImportationUIBase::DebugCallConvert()
{
	TArray<uint8> MP3Data;
	TArray<uint8> OGGData;
	if (!FFileHelper::LoadFileToArray(MP3Data, *MusicFileName))
		return nullptr;
	if (!UMuthMBPLib::ConvertMP3ToOGG(MP3Data, OGGData))
		return nullptr;
	return UMuthMBPLib::DecodeWaveFromOGG(OGGData);
}
