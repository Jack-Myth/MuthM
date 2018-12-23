// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(MuthMNativeLib,Log,All)
class MuthMNativeLib
{
public:

	FORCEINLINE static float _FFTHannWindow(const int16& SampleValue, int SampleIndex, int SampleCount)
	{
		return SampleValue * 0.5f * (1 - FMath::Cos(2 * PI * SampleIndex / (SampleCount - 1)));
	}
	static bool NativeDecodeMP3ToPCM(const TArray<uint8>& _MP3Data, TArray<uint8>& OutputPCM, int32& SampleRate,int32& Channels);
	static bool NativeEncodePCMToOGG(const TArray<uint8>& PCMData, int32 SampleRate, int32 Channels/*, int32 TargetBitrate = 192*1024*/, TArray<uint8>& OutputOGG);
	static void NativeCalculateFrequencySpectrum(const TArray<uint8>& PCMInput, int Channels, bool SplitChannels, const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<TArray<float>>& OutSpectrums);
	static float NativeDetectBPMFromPCM(const TArray<uint8>& PCMInput,int32 SampleRate, int32 Channels);
};
