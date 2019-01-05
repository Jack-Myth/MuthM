// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainSoundWave.h"
#include "MuthMNativeLib.h"

bool UMainSoundWave::DecodePCMFromCompressedData(TArray<uint8>& OutputPCM)
{
	if (RawPCMData)
	{
		OutputPCM.SetNum(RawPCMDataSize);
		FMemory::Memcpy(OutputPCM.GetData(), RawPCMData,RawPCMDataSize);
		return true;
	}
	FByteBulkData* CompressedDataBulk = GetCompressedData(TEXT("OGG"));
	if (!CompressedDataBulk)
		return false; //No Compressed Data,No way to decode.
	const uint8* pCompressedData = (const uint8*)CompressedDataBulk->LockReadOnly();
	int s, c; //Just used to hold return value,I use UMainSoundWave::Samplerate to get the real info.
	bool IsDecodeSuccessful;
	IsDecodeSuccessful=MuthMNativeLib::DecodeOGGToPCM(pCompressedData, CompressedDataBulk->GetBulkDataSize(), OutputPCM, s, c);
	CompressedDataBulk->Unlock();
	UE_LOG(LogTemp, Display, TEXT("PCMDecodeResult:%d,Size:%d"), IsDecodeSuccessful ? 1 : 0, OutputPCM.Num());
	return IsDecodeSuccessful;
}
