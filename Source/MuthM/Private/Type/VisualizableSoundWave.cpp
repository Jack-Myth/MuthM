// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "VisualizableSoundWave.h"
#include "MuthMNativeLib.h"

void UVisualizableSoundWave::CalculateFrequencySpectrum(bool SplitChannels, const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<TArray<float>>& OutSpectrums)
{
	MuthMNativeLib::NativeCalculateFrequencySpectrum(_CachedStdPCM, this->NumChannels, SplitChannels, BeginTime, SampleTimeLength, SpectrumWidth, OutSpectrums);
}

void UVisualizableSoundWave::CalculateFrequencySpectrumSplit(const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<float>& LSpectrum, TArray<float>& RSpectrum)
{
	TArray<TArray<float>> tmpSpectrums;
	CalculateFrequencySpectrum(true, BeginTime, SampleTimeLength, SpectrumWidth, tmpSpectrums);
	LSpectrum = tmpSpectrums[0];
	RSpectrum = tmpSpectrums[1];
}

void UVisualizableSoundWave::CalculateFrequencySpectrumMixed(const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<float>& MixedSpectrum)
{
	TArray<TArray<float>> tmpSpectrums;
	CalculateFrequencySpectrum(true, BeginTime, SampleTimeLength, SpectrumWidth, tmpSpectrums);
	MixedSpectrum = tmpSpectrums[0];
}

float UVisualizableSoundWave::CalculateBPM()
{
	return MuthMNativeLib::NativeDetectBPMFromPCM(_CachedStdPCM,SampleRate, this->NumChannels);
}
