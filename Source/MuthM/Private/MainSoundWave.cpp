// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainSoundWave.h"
#include "MuthMNativeLib.h"

bool UMainSoundWave::DecodePCMFromCompressedData(TArray<uint8>& OutputPCM)
{
	FByteBulkData* CompressedDataBulk = GetCompressedData("OGG");
	if (!CompressedDataBulk)
		return false; //No Compressed Data,No way to decode.
	const uint8* pCompressedData = (const uint8*)CompressedDataBulk->LockReadOnly();
	int s, c; //Just used to hold return value,I use UMainSoundWave::Samplerate to get the real info.
	bool IsDecodeSuccessful;
	IsDecodeSuccessful=MuthMNativeLib::DecodeOGGToPCM(pCompressedData, CompressedDataBulk->GetBulkDataSize(), OutputPCM, s, c);
	CompressedDataBulk->Unlock();
	return IsDecodeSuccessful;
}
