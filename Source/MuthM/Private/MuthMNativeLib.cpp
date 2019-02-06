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
#include "fmod.hpp"
#include <algorithm>

DEFINE_LOG_CATEGORY(MuthMNativeLib)

class FMOD::System* MuthMNativeLib::pFModSystemSingleton = nullptr;
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

float MuthMNativeLib::NativeDetectBPMFromPCM(const TArray<uint8>& PCMInput,int32 SampleRate, int32 Channels)
{
	//Because <³õÒô¥ß¥¯¤ÎÏûÊ§> is far beyond 200BPM,so set a higher value
	//Maybe is not necessary.
	BeatDetektor m_BeatDetektor(50.f,300.f);
	m_BeatDetektor.reset();
	float MusicLength = int(PCMInput.Num() / sizeof(int16) / Channels / SampleRate);
	TArray<TArray<float>> FFTValue;
	int Sampleperframe = 768;
	float Secondperframe = (float)Sampleperframe / (float)SampleRate;
	std::vector<float> stdv;
	stdv.resize(Sampleperframe * 2);
	for (float i = 0; i < MusicLength; i+= Secondperframe)
	{
		NativeCalculateFrequencySpectrum(PCMInput, SampleRate, Channels, false, i, Secondperframe, Sampleperframe, FFTValue);
		bool abandon = true;
		for (int k = 0; k < 64; k++)
		{
			if (FMath::IsFinite(FFTValue[0][k]))
			{
				abandon = false;
				break;
			}
		}
		if (abandon)
		{
			FMemory::Memzero(stdv.data(), stdv.size());
		}
		else
		{
			FMemory::Memcpy(stdv.data(), FFTValue[0].GetData(), FFTValue[0].Num());
			std::reverse_copy(FFTValue[0].GetData(), FFTValue[0].GetData() + FFTValue[0].Num(), stdv.data() + FFTValue[0].Num());
		}
		std::reverse_copy(FFTValue[0].GetData(), FFTValue[0].GetData() + FFTValue[0].Num(), stdv.data());
		FMemory::Memcpy(stdv.data() + FFTValue[0].Num(), FFTValue[0].GetData(), FFTValue[0].Num());
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

FMOD::System* MuthMNativeLib::GetFModSystem()
{
	if (!pFModSystemSingleton)
	{
		FMOD::System_Create(&pFModSystemSingleton);
		pFModSystemSingleton->init(2, FMOD_INIT_NORMAL, nullptr);
	}
	return pFModSystemSingleton;
}

void MuthMNativeLib::NativeCalculateFrequencySpectrum(const TArray<uint8>& PCMData,const int SampleRate,const int NumChannels, const bool bSplitChannels, const float StartTime, const float TimeLength, const int32 SpectrumWidth, TArray< TArray<float> >& OutSpectrums)
{

	OutSpectrums.Empty();
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
			const uint8* RawWaveData = PCMData.GetData();
			int32 RawDataSize = PCMData.Num();

			int32 SampleCount = 0;
			int32 SampleCounts[10] = { 0 };

			int32 FirstSample = SampleRate * StartTime;
			int32 LastSample = SampleRate * (StartTime + TimeLength);

			SampleCount = RawDataSize / (2 * NumChannels);

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
						buf[ChannelIndex][SampleIndex].r = _FFTHannWindow(*SamplePtr, SampleIndex, SamplesToRead);
						buf[ChannelIndex][SampleIndex].i = 0.f;

						SamplePtr++;
					}
				}

				for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
				{
					kiss_fftnd(stf, buf[ChannelIndex], out[ChannelIndex]);
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