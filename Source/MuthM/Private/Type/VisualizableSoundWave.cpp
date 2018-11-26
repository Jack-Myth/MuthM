// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "VisualizableSoundWave.h"
#include "tools/kiss_fftnd.h"


void UVisualizableSoundWave::CalculateFrequencySpectrum(bool SplitChannels, const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<TArray<float>> OutSpectrums)
{
	OutSpectrums.Empty();
	constexpr int SampleRate = 48000;
	int BeginSampleIndex = BeginTime * SampleRate;
	int EndSampleIndex = (BeginTime + SampleTimeLength)*SampleRate;
	int SampleCount = EndSampleIndex - BeginSampleIndex;
	TArray<int16> StdSampleData;
	StdSampleData.SetNum(SampleCount * 2);
	FMemory::Memcpy(StdSampleData.GetData(), _CachedStdPCM.GetData() + BeginSampleIndex, SampleCount * sizeof(uint16) * 2);
	int ChannelCount = 2;
	if (SplitChannels)
	{
		for (int i = 0; i < StdSampleData.Num()*0.5f; i++)
			StdSampleData[i] = ((int)StdSampleData[2 * i] + StdSampleData[2 * i + 1])*0.5f;
		StdSampleData.SetNum(StdSampleData.Num()*0.5f);
		ChannelCount = 1;
	}
	kiss_fft_cpx* inbuf[2] = { NULL };
	kiss_fft_cpx* outbuf[2] = { NULL };
	int32 Dims[1] = { SampleCount };
	kiss_fftnd_cfg fftcfg = kiss_fftnd_alloc(Dims, 1, 0, NULL, NULL);
	for (int i = 0; i < ChannelCount; i++)
	{
		inbuf[i] = (kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*SampleCount);
		outbuf[i] = (kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*SampleCount);
	}
	for (int i = 0; i < SampleCount; i++)
	{
		for (int cc=0; cc < ChannelCount; cc++)
		{
			inbuf[cc][i].r = _FFTHannWindow(StdSampleData[i],i,SampleCount);
			inbuf[cc][i].i = 0.f;
		}
	}
	OutSpectrums.SetNum(ChannelCount);
	for (int cc=0;cc<ChannelCount;cc++)
	{
		kiss_fftnd(fftcfg, inbuf[cc], outbuf[cc]);
		int32 SamplesPerSpectrum = SampleCount / (2 * SpectrumWidth);
		int32 ExcessSamples = SampleCount % (2 * SpectrumWidth);
		int BeginIndex = 1;
		for (int SpectrumIndex = 0; SpectrumIndex < SpectrumWidth; SpectrumIndex++)
		{
			int32 SamplesForSpectrum = SamplesPerSpectrum + (ExcessSamples-- > 0 ? 1 : 0);
			float SampleVal = 0;
			for (int SpectrumSampleIndex = 0; SpectrumSampleIndex < SamplesForSpectrum; SpectrumSampleIndex++)
			{
				float PostScaledR = outbuf[cc][BeginIndex + SpectrumSampleIndex].r * 2.f / SampleCount;
				float PostScaledI = outbuf[cc][BeginIndex + SpectrumSampleIndex].i * 2.f / SampleCount;
				SampleVal+= 10.f * FMath::LogX(10.f, FMath::Square(PostScaledR) + FMath::Square(PostScaledI));
			}
			SampleVal /= SpectrumWidth;
			OutSpectrums[cc][SpectrumIndex] = SampleVal;
			BeginIndex += SamplesForSpectrum;
		}
		KISS_FFT_FREE(inbuf[cc]);
		KISS_FFT_FREE(outbuf[cc]);
	}
	KISS_FFT_FREE(fftcfg);
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
