// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicManagerImpl.h"
#include "FileHelper.h"
#include "Paths.h"
#include "BulkData.h"
#include "Sound/SoundWave.h"
#include "OpusAudioInfo.h"
#include "TargetPlatform/Public/Interfaces/IAudioFormat.h"

DEFINE_LOG_CATEGORY(MuthMMusicManager)

bool UMusicManagerImpl::LoadMusicDataByID(int MusicID, TArray<uint8>& MusicData)
{
	FString MusicFileName = FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), "/__Music/",FString::FromInt(MusicID)+".Opus");
	return FFileHelper::LoadFileToArray(MusicData,*MusicFileName);
}

bool UMusicManagerImpl::FindMusicOnlineByID(int MusicID, FMusicInfo& MusicInfo)
{
	//TODO: FindMusicOnlineByID
	return false;
}

bool UMusicManagerImpl::DownloadMusicByID(int MusicID)
{
	//TODO:DownloadMusicByID
	return false;
}

bool UMusicManagerImpl::IsMusicExistInLocal(int MusicID)
{
	return false;
}
