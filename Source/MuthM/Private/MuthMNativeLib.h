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
	/**
	 * StdPCM Info: 48000Hz,16Bit,2Channels
	 * This kind of PCM can be convert to Opus Directly.
	 * Easy to Use,so decide to use it as Stranded PCM.
	 */

	FORCEINLINE static float _FFTHannWindow(const int16& SampleValue, int SampleIndex, int SampleCount)
	{
		return SampleValue * 0.5f * (1 - FMath::Cos(2 * PI * SampleIndex / (SampleCount - 1)));
	}
	static bool NativeDecodeMP3ToStdPCM(const TArray<uint8>& _MP3Data, TArray<uint8>& OutputStdPCM);
	static bool NativeEncodeStdPCMToOpus(const TArray<uint8>& _StdPCM, TArray<uint8>& OutputOpus);
	static void NativeCalculateFrequencySpectrum(const TArray<uint8>& _StdPCMInput, bool SplitChannels, const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<TArray<float>>& OutSpectrums);
	static float NativeDetectBPMFromPCM(const TArray<uint8>& _StdPCMInput);
};
