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

class USoundWave* UMusicManagerImpl::GenSoundWaveByOpus(const TArray<uint8>& OpusData)
{
	USoundWave* TargetSoundWave = NewObject<USoundWave>();
	FByteBulkData* bulkData = &TargetSoundWave->CompressedFormatData.GetFormat(TEXT("Opus"));
	bulkData->Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(bulkData->Realloc(OpusData.Num()), OpusData.GetData(), OpusData.Num());
	bulkData->Unlock();
	FSoundQualityInfo soundQualityInfo;
	FOpusAudioInfo opusAudioInfo;
	if (!opusAudioInfo.ReadCompressedInfo(OpusData.GetData(), OpusData.Num(), &soundQualityInfo))
	{
		UE_LOG(MuthMMusicManager, Error, TEXT("Unable to read Opus Info"));
		return nullptr;
	}
	TargetSoundWave->SoundGroup = ESoundGroup::SOUNDGROUP_Default;
	TargetSoundWave->NumChannels = soundQualityInfo.NumChannels;
	TargetSoundWave->Duration = soundQualityInfo.Duration;
	TargetSoundWave->RawPCMDataSize = soundQualityInfo.SampleDataSize;
	TargetSoundWave->SetSampleRate(soundQualityInfo.SampleRate);
	return TargetSoundWave;
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
