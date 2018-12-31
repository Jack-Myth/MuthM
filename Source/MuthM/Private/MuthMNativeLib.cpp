// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMNativeLib.h"
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"
#include "LogMacros.h"
#include "AudioResampler.h"
#include "BeatDetektor.h"
#include "tools/kiss_fftnd.h"
#include <vector>
#include "OpusAudioInfo.h"
#include "VorbisAudioInfo.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#include "FileHelper.h"
#include "Paths.h"

DEFINE_LOG_CATEGORY(MuthMNativeLib)
//Copy from Unreal Engine 4 Internal Resample function :P
static void ResampleWaveData(TArray<uint8>& WaveData, size_t& NumBytes, int32 NumChannels, float SourceSampleRate, float DestinationSampleRate)
{
	double StartTime = FPlatformTime::Seconds();

	// Set up temporary output buffers:
	Audio::AlignedFloatBuffer ResamplerInputData;
	Audio::AlignedFloatBuffer ResamplerOutputData;

	int32 NumSamples = NumBytes / sizeof(int16);

	check(WaveData.Num() == NumBytes);
	check(NumSamples == NumBytes / 2);

	// Convert wav data from int16 to float:
	ResamplerInputData.AddUninitialized(NumSamples);
	int16* InputData = (int16*)WaveData.GetData();

	for (int32 Index = 0; Index < NumSamples; Index++)
	{
		ResamplerInputData[Index] = ((float)InputData[Index]) / 32767.0f;
	}

	// set up converter input params:
	Audio::FResamplingParameters ResamplerParams = {
		Audio::EResamplingMethod::BestSinc,
		NumChannels,
		SourceSampleRate,
		DestinationSampleRate,
		ResamplerInputData
	};

	// Allocate enough space in output buffer for the resulting audio:
	ResamplerOutputData.AddUninitialized(Audio::GetOutputBufferSize(ResamplerParams));
	Audio::FResamplerResults ResamplerResults;
	ResamplerResults.OutBuffer = &ResamplerOutputData;

	// Resample:
	if (Audio::Resample(ResamplerParams, ResamplerResults))
	{
		// resize WaveData buffer and convert back to int16:
		int32 NumSamplesGenerated = ResamplerResults.OutputFramesGenerated * NumChannels;
		WaveData.SetNum(NumSamplesGenerated * sizeof(int16));
		InputData = (int16*)WaveData.GetData();

		for (int32 Index = 0; Index < NumSamplesGenerated; Index++)
		{
			InputData[Index] = (int16)(ResamplerOutputData[Index] * 32767.0f);
		}

		NumBytes = NumSamplesGenerated * sizeof(int16);
	}
	else
	{
		UE_LOG(MuthMNativeLib, Error, TEXT("Resampling operation failed."));
	}

	double TimeDelta = FPlatformTime::Seconds() - StartTime;
	UE_LOG(MuthMNativeLib, Display, TEXT("Resampling file from %f to %f took %f seconds."), SourceSampleRate, DestinationSampleRate, TimeDelta);
}

bool MuthMNativeLib::NativeDecodeMP3ToPCM(const TArray<uint8>& _MP3Data, TArray<uint8>& OutputPCM, int32& SampleRate, int32& Channels)
{
	if (!_MP3Data.Num())
		return false;
	OutputPCM.Empty();
	//Begin MP3 Decode
	mp3dec_file_info_t mp3FileInfo = {NULL};
	mp3dec_t MiniMp3Dec;
	mp3dec_init(&MiniMp3Dec);
	mp3dec_load_buf(&MiniMp3Dec, _MP3Data.GetData(), _MP3Data.Num(), &mp3FileInfo, nullptr, nullptr);
	if (!mp3FileInfo.samples)
		return false;
	//Resample
	OutputPCM.Append((const uint8*)mp3FileInfo.buffer, mp3FileInfo.samples * 2); //sizeof(int16_t)==2*sizeof(uint8)
	free(mp3FileInfo.buffer);
	SampleRate = mp3FileInfo.hz;
	Channels = mp3FileInfo.channels;
	return true;
}

bool MuthMNativeLib::NativeEncodePCMToOGG(const TArray<uint8>& PCMData, int32 SampleRate, int32 Channels/*, int32 TargetBitrate = 192*1024*/, TArray<uint8>& OutputOGG)
{
	OutputOGG.Empty();
	//Init Vorbis 
	vorbis_info vi;
	vorbis_dsp_state vd;
	vorbis_comment vc;
	vorbis_block vb;
	ogg_stream_state os;
	ogg_packet header,header_comment,header_code;
	ogg_packet op;
	ogg_page og;
	vorbis_info_init(&vi);
	vorbis_encode_init(&vi, 2, SampleRate, -1, 192000, -1);
	vorbis_analysis_init(&vd, &vi);
	vorbis_comment_init(&vc);
	vorbis_block_init(&vd, &vb);
	ogg_stream_init(&os, FMath::Rand());
	vorbis_analysis_headerout(&vd, &vc, &header, &header_comment, &header_code);
	ogg_stream_packetin(&os, &header);
	ogg_stream_packetin(&os, &header_comment);
	ogg_stream_packetin(&os, &header_code);
	while (ogg_stream_flush(&os, &og))
	{
		OutputOGG.Append(og.header, og.header_len);
		OutputOGG.Append(og.body, og.body_len);
	}
	int sampleCount = PCMData.Num() / Channels / sizeof(uint16);
	int targetSampleCount; 
	int processedSample = 0;
	constexpr int BlockSize = 102400;
	while (sampleCount)
	{
		targetSampleCount = sampleCount > BlockSize ? BlockSize : sampleCount;
		float** PCMBuffer = vorbis_analysis_buffer(&vd, targetSampleCount);
		for (int i = 0; i < targetSampleCount; i++)
		{
			for (int c = 0; c < Channels; c++)
			{
				//XXX: Big-Little endian waring
				PCMBuffer[c][i] = (*(int16*)(PCMData.GetData() + ((i + processedSample) *Channels + c)* (sizeof(uint16))))/32768.f;
			}
		}
		vorbis_analysis_wrote(&vd, targetSampleCount);
		sampleCount -= targetSampleCount;
		processedSample += targetSampleCount;
	}
	vorbis_analysis_wrote(&vd, 0);
	while (vorbis_analysis_blockout(&vd, &vb) == 1)
	{
		vorbis_analysis(&vb, NULL);
		vorbis_bitrate_addblock(&vb);
		while (vorbis_bitrate_flushpacket(&vd, &op))
		{
			ogg_stream_packetin(&os, &op);
			while (ogg_stream_pageout(&os, &og))
			{
				OutputOGG.Append(og.header, og.header_len);
				OutputOGG.Append(og.body, og.body_len);
				if (ogg_page_eos(&og)) break;  //TODO: Really needed?
			}
		}
	}
	//Clean Resources
	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
	return true;
}

void MuthMNativeLib::NativeCalculateFrequencySpectrum(const TArray<uint8>& PCMInput, int SampleRate, int Channels, bool SplitChannels, const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<TArray<float>>& OutSpectrums)
{
	OutSpectrums.Empty();
	int BeginSampleIndex = BeginTime * SampleRate;
	int EndSampleIndex = (BeginTime + SampleTimeLength)*SampleRate;
	int SampleCount = EndSampleIndex - BeginSampleIndex;
	TArray<int16> OriginalSampleData;
	OriginalSampleData.SetNum(SampleCount * 2);
	FMemory::Memcpy(OriginalSampleData.GetData(), PCMInput.GetData() + BeginSampleIndex, SampleCount * sizeof(uint16) * 2);
	if (SplitChannels)
	{
		for (int i = 0; i < OriginalSampleData.Num()*0.5f; i++)
			OriginalSampleData[i] = ((int)OriginalSampleData[2 * i] + OriginalSampleData[2 * i + 1])*0.5f;
		OriginalSampleData.SetNum(OriginalSampleData.Num()*0.5f);
		Channels = 1;
	}
	kiss_fft_cpx* inbuf[2] = { NULL };
	kiss_fft_cpx* outbuf[2] = { NULL };
	int32 Dims[1] = { SampleCount };
	kiss_fftnd_cfg fftcfg = kiss_fftnd_alloc(Dims, 1, 0, NULL, NULL);
	for (int i = 0; i < Channels; i++)
	{
		inbuf[i] = (kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*SampleCount);
		outbuf[i] = (kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*SampleCount);
	}
	for (int i = 0; i < SampleCount; i++)
	{
		for (int cc = 0; cc < Channels; cc++)
		{
			inbuf[cc][i].r = _FFTHannWindow(OriginalSampleData[i], i, SampleCount);
			inbuf[cc][i].i = 0.f;
		}
	}
	OutSpectrums.SetNum(Channels);
	for (int i = 0; i < Channels; i++)
		OutSpectrums[i].SetNum(SpectrumWidth);
	for (int cc = 0; cc < Channels; cc++)
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
				SampleVal += 10.f * FMath::LogX(10.f, FMath::Square(PostScaledR) + FMath::Square(PostScaledI));
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

float MuthMNativeLib::NativeDetectBPMFromPCM(const TArray<uint8>& PCMInput,int32 SampleRate, int32 Channels)
{
	//Because <³õÒô¥ß¥¯¤ÎÏûÊ§> is far beyond 200BPM,so set a higher value
	//Maybe is not necessary.
	//UNDONE: BeatDetektor Now have a big issue,so return 100 directly.
	return 100;
	BeatDetektor m_BeatDetektor(100.f,300.f);
	m_BeatDetektor.reset();
	float MusicLength = int(PCMInput.Num() / SampleRate);
	TArray<TArray<float>> FFTValue;
	int Sampleperframe = 2048;
	float Secondperframe = (float)Sampleperframe / SampleRate;
	for (float i = 0; i < MusicLength; i+= Secondperframe)
	{
		NativeCalculateFrequencySpectrum(PCMInput, SampleRate, Channels, false, i, Secondperframe, Sampleperframe, FFTValue);
		std::vector<float> stdv;
		stdv.assign(FFTValue[0].GetData(), FFTValue[0].GetData()+FFTValue[0].Num());
		m_BeatDetektor.process(i,stdv);
	}
	return m_BeatDetektor.win_bpm_int_lo;
}

bool MuthMNativeLib::DecodeOGGToPCM(const uint8* pOGGData, int32 OGGDataLength, TArray<uint8>& PCMData, int& SampleRate, int& Channels)
{
	PCMData.Empty();
	FVorbisAudioInfo tmpVorbisAudioInfo;
	//XXX: It may not be a right way.
	FSoundQualityInfo OGGSoundQualityInfo;
	tmpVorbisAudioInfo.ReadCompressedInfo(pOGGData, OGGDataLength, &OGGSoundQualityInfo);
	PCMData.SetNum(OGGSoundQualityInfo.SampleDataSize);
	SampleRate = OGGSoundQualityInfo.SampleRate;
	Channels = OGGSoundQualityInfo.NumChannels;
	tmpVorbisAudioInfo.ExpandFile(PCMData.GetData(), &OGGSoundQualityInfo);
	return true;
}








//////////////////////////////////////////////////////////////////////////


float GetFFTInValue(const int16 SampleValue, const int16 SampleIndex, const int16 SampleCount)
{
	float FFTValue = SampleValue;

	// Apply the Hann window
	FFTValue *= 0.5f * (1 - FMath::Cos(2 * PI * SampleIndex / (SampleCount - 1)));

	return FFTValue;
}

void CalculateFrequencySpectrum(TArray<uint8>& PCMData,int Channels, const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 SpectrumWidth, TArray< TArray<float> >& OutSpectrums)
{

	OutSpectrums.Empty();

#if WITH_EDITORONLY_DATA
	const int32 NumChannels = Channels;
	if (SpectrumWidth > 0 && NumChannels > 0)
	{
		// Setup the output data
		OutSpectrums.AddZeroed((bSplitChannels ? NumChannels : 1));
		for (int32 ChannelIndex = 0; ChannelIndex < OutSpectrums.Num(); ++ChannelIndex)
		{
			OutSpectrums[ChannelIndex].AddZeroed(SpectrumWidth);
		}

		// check if there is any raw sound data
		if (PCMData.Num() > 0)
		{
			// Lock raw wave data.
			uint8* RawWaveData = PCMData.GetData();
			int32 RawDataSize = PCMData.Num();
			FWaveModInfo WaveInfo;

			uint32 SamplesPerSec = 44100;
			WaveInfo.SampleDataSize = PCMData.Num();
			WaveInfo.pSamplesPerSec = &SamplesPerSec;

			// parse the wave data
			if (1)
			{
				int32 SampleCount = 0;
				int32 SampleCounts[10] = { 0 };

				int32 FirstSample = *WaveInfo.pSamplesPerSec * StartTime;
				int32 LastSample = *WaveInfo.pSamplesPerSec * (StartTime + TimeLength);

				if (NumChannels <= 2)
				{
					SampleCount = WaveInfo.SampleDataSize / (2 * NumChannels);
				}
				else
				{
					for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
					{
						SampleCounts[ChannelIndex] = (PCMData.Num()/2 / NumChannels);
						SampleCount = FMath::Max(SampleCount, SampleCounts[ChannelIndex]);
						SampleCounts[ChannelIndex] -= FirstSample;
					}
				}

				FirstSample = FMath::Min(SampleCount, FirstSample);
				LastSample = FMath::Min(SampleCount, LastSample);

				int32 SamplesToRead = LastSample - FirstSample;

				if (SamplesToRead > 0)
				{
					// Shift the window enough so that we get a power of 2
					int32 PoT = 2;
					while (SamplesToRead > PoT) PoT *= 2;
					FirstSample = FMath::Max(0, FirstSample - (PoT - SamplesToRead) / 2);
					SamplesToRead = PoT;
					LastSample = FirstSample + SamplesToRead;
					if (LastSample > SampleCount)
					{
						FirstSample = LastSample - SamplesToRead;
					}
					if (FirstSample < 0)
					{
						// If we get to this point we can't create a reasonable window so just give up
						return;
					}

					kiss_fft_cpx* buf[10] = { 0 };
					kiss_fft_cpx* out[10] = { 0 };

					int32 Dims[1] = { SamplesToRead };
					kiss_fftnd_cfg stf = kiss_fftnd_alloc(Dims, 1, 0, NULL, NULL);


					const int16* SamplePtr = reinterpret_cast<const int16*>(PCMData.GetData());
					if (NumChannels <= 2)
					{
						for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
						{
							buf[ChannelIndex] = (kiss_fft_cpx *)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx) * SamplesToRead);
							out[ChannelIndex] = (kiss_fft_cpx *)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx) * SamplesToRead);
						}

						SamplePtr += (FirstSample * NumChannels);

						for (int32 SampleIndex = 0; SampleIndex < SamplesToRead; ++SampleIndex)
						{
							for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
							{
								buf[ChannelIndex][SampleIndex].r = GetFFTInValue(*SamplePtr, SampleIndex, SamplesToRead);
								buf[ChannelIndex][SampleIndex].i = 0.f;

								SamplePtr++;
							}
						}
					}
					else
					{
						for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
						{
							// Drop this channel out if there isn't the power of 2 number of samples available
							if (SampleCounts[ChannelIndex] >= SamplesToRead)
							{
								buf[ChannelIndex] = (kiss_fft_cpx *)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx) * SamplesToRead);
								out[ChannelIndex] = (kiss_fft_cpx *)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx) * SamplesToRead);

								for (int32 SampleIndex = 0; SampleIndex < SamplesToRead; ++SampleIndex)
								{
									buf[ChannelIndex][SampleIndex].r = GetFFTInValue(*(SamplePtr + FirstSample + SampleIndex /*+ SoundWave->ChannelOffsets[ChannelIndex] / 2*/), SampleIndex, SamplesToRead);
									buf[ChannelIndex][SampleIndex].i = 0.f;
								}
							}
						}
					}

					for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
					{
						if (buf[ChannelIndex])
						{
							kiss_fftnd(stf, buf[ChannelIndex], out[ChannelIndex]);
						}
					}

					int32 SamplesPerSpectrum = SamplesToRead / (2 * SpectrumWidth);
					int32 ExcessSamples = SamplesToRead % (2 * SpectrumWidth);

					int32 FirstSampleForSpectrum = 1;
					for (int32 SpectrumIndex = 0; SpectrumIndex < SpectrumWidth; ++SpectrumIndex)
					{
						static bool doLog = false;

						int32 SamplesRead = 0;
						double SampleSum = 0;
						int32 SamplesForSpectrum = SamplesPerSpectrum + (ExcessSamples-- > 0 ? 1 : 0);
						for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
						{
							if (out[ChannelIndex])
							{
								if (bSplitChannels)
								{
									SampleSum = 0;
								}

								for (int32 SampleIndex = 0; SampleIndex < SamplesForSpectrum; ++SampleIndex)
								{
									float PostScaledR = out[ChannelIndex][FirstSampleForSpectrum + SampleIndex].r * 2.f / SamplesToRead;
									float PostScaledI = out[ChannelIndex][FirstSampleForSpectrum + SampleIndex].i * 2.f / SamplesToRead;
									//float Val = FMath::Sqrt(FMath::Square(PostScaledR) + FMath::Square(PostScaledI));
									float Val = 10.f * FMath::LogX(10.f, FMath::Square(PostScaledR) + FMath::Square(PostScaledI));
									SampleSum += Val;
								}

								if (bSplitChannels)
								{
									OutSpectrums[ChannelIndex][SpectrumIndex] = (float)(SampleSum / SamplesForSpectrum);
								}
								SamplesRead += SamplesForSpectrum;
							}
						}

						if (!bSplitChannels)
						{
							OutSpectrums[0][SpectrumIndex] = (float)(SampleSum / SamplesRead);
						}

						FirstSampleForSpectrum += SamplesForSpectrum;
					}

					KISS_FFT_FREE(stf);
					for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
					{
						if (buf[ChannelIndex])
						{
							KISS_FFT_FREE(buf[ChannelIndex]);
							KISS_FFT_FREE(out[ChannelIndex]);
						}
					}
				}
			}
		}
	}
#else
	UE_LOG(LogSoundVisualization, Warning, TEXT("Calculate Frequency Spectrum does not work for cooked builds yet."));
#endif
}

//////////////////////////////////////////////////////////////////////////