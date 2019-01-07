// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainSoundWaveImplFMod.h"
#include "MuthMNativeLib.h"
#include "fmod.hpp"

bool UMainSoundWaveImplFMod::GenPCMData(TArray<uint8>& OutputPCM)
{
	uint32 PCMByteLength;
	pFModSound->getLength(&PCMByteLength, FMOD_TIMEUNIT_PCMBYTES);
	uint8* pBuf1,*pBuf2;
	uint32 Buf1Length, Buf2Length;
	if (pFModSound->lock(0, PCMByteLength, (void**)&pBuf1, (void**)&pBuf2, &Buf1Length, &Buf2Length) == FMOD_OK)
	{
		OutputPCM.SetNum(PCMByteLength);
		FMemory::Memcpy(OutputPCM.GetData(), pBuf1, Buf1Length);
		pFModSound->unlock(pBuf1, pBuf2, Buf1Length, Buf2Length);
		return true;
	}
	return false;
}

float UMainSoundWaveImplFMod::GetSoundDuration() const
{
	uint32 MSLength;
	pFModSound->getLength(&MSLength, FMOD_TIMEUNIT_MS);
	return MSLength / 1000.f;
}

int32 UMainSoundWaveImplFMod::GetSampleRate() const
{
	uint32 SampleCount;
	pFModSound->getLength(&SampleCount, FMOD_TIMEUNIT_PCM);
	return SampleCount / GetSoundDuration();
}

int32 UMainSoundWaveImplFMod::GetNumChannels() const
{
	int32 Channels = 0;
	pFModSound->getFormat(nullptr, nullptr, &Channels, nullptr);
	return Channels;
}

UMainSoundWaveImplFMod::~UMainSoundWaveImplFMod()
{
	pFModSound.Reset();
}
