// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//Copy from IAudioFormat.h
struct FSoundQualityInfo
{
	/** Holds the quality value ranging from 1 [poor] to 100 [very good]. */
	int32 Quality;

	/** Holds the number of distinct audio channels. */
	uint32 NumChannels;

	/** Holds the number of PCM samples per second. */
	uint32 SampleRate;

	/** Holds the size of sample data in bytes. */
	uint32 SampleDataSize;

	/** Holds the length of the sound in seconds. */
	float Duration;

	/** Holds a string for debugging purposes. */
	FString DebugName;
};

#ifdef _MUTHM_USE_FMOD
namespace FMOD
{
	class System;
}
#endif

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(MuthMNativeLib,Log,All)
class MuthMNativeLib
{

#ifdef _MUTHM_USE_FMOD
	static class FMOD::System* pFModSystemSingleton;
#endif
public:

	static float _FFTHannWindow(const int16& SampleValue, int SampleIndex, int SampleCount)
	{
		return SampleValue * 0.5f * (1 - FMath::Cos(2 * PI * SampleIndex / (SampleCount - 1)));
	}
	static bool NativeDecodeMP3ToPCM(const TArray<uint8>& _MP3Data, TArray<uint8>& OutputPCM, int32& SampleRate,int32& Channels);
	static bool NativeEncodePCMToOGG(const TArray<uint8>& PCMData, int32 SampleRate, int32 Channels/*, int32 TargetBitrate = 192*1024*/, TArray<uint8>& OutputOGG);
	static void NativeCalculateFrequencySpectrum(const TArray<uint8>& PCMData, const int SampleRate, const int NumChannels, const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 SpectrumWidth, TArray< TArray<float> >& OutSpectrums);
	static float NativeDetectBPMFromPCM(const TArray<uint8>& PCMInput,int32 SampleRate, int32 Channels);
	static bool DecodeOGGToPCM(const uint8* pOGGData,int32 OGGDataLength, TArray<uint8>& PCMData,int& SampleRate,int& Channels);
#ifdef _MUTHM_USE_FMOD
	static FMOD::System* GetFModSystem();
#endif
};
