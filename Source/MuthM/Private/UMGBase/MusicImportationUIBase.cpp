// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicImportationUIBase.h"
#include "MusicManager.h"
#include "FileHelper.h"
#include "MuthMBPLib.h"

void UMusicImportationUIBase::BeginImportMusic(const FString& Title, const FString& Musician)
{
	//XXX:Treat all music file as MP3.
	IMusicManager::Get(this)->ImportMP3(MusicFileName, Title, Musician);
	OnMusicImportFinished.ExecuteIfBound();
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
