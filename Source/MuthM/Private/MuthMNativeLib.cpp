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
	vorbis_encode_init(&vi, 2, SampleRate, 192*1024, 168*1024, 0);
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
	float** PCMBuffer = vorbis_analysis_buffer(&vd, sampleCount);
	for (int i=0;i<sampleCount;i++)
	{
		for (int c=0;c<Channels;c++)
		{
			//XXX: Big-Little endian waring
			PCMBuffer[c][i] = *(uint16*)(PCMData.GetData() + (i *Channels + c)* (sizeof(uint16)));
		}
	}
	vorbis_analysis_wrote(&vd, sampleCount);
	while (vorbis_analysis_blockout(&vd,&vb)==1)
	{
		vorbis_analysis(&vb, &op);
		vorbis_bitrate_addblock(&vb);
		while (vorbis_bitrate_flushpacket(&vd,&op))
		{
			ogg_stream_packetin(&os, &op);
			while (ogg_stream_pageout(&os,&og))
			{
				OutputOGG.Append(og.header, og.header_len);
				OutputOGG.Append(og.body, og.body_len);
			}
		}
	}
	vorbis_analysis_wrote(&vd, 0);
	while (vorbis_analysis_blockout(&vd, &vb) == 1)
	{
		vorbis_analysis(&vb, &op);
		vorbis_bitrate_addblock(&vb);
		while (vorbis_bitrate_flushpacket(&vd, &op))
		{
			ogg_stream_packetin(&os, &op);
			while (ogg_stream_pageout(&os, &og))
			{
				OutputOGG.Append(og.header, og.header_len);
				OutputOGG.Append(og.body, og.body_len);
			}
		}
	}
	//Clean Resources
	ogg_packet_clear(&op);
	ogg_packet_clear(&header);
	ogg_packet_clear(&header_comment);
	ogg_packet_clear(&header_code);
	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
	return true;
}

void MuthMNativeLib::NativeCalculateFrequencySpectrum(const TArray<uint8>& PCMInput, int Channels, bool SplitChannels, const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<TArray<float>>& OutSpectrums)
{
	OutSpectrums.Empty();
	constexpr int SampleRate = 48000;
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
	BeatDetektor m_BeatDetektor(100.f,300.f);
	m_BeatDetektor.reset();
	float MusicLength = int(PCMInput.Num() / SampleRate);
	TArray<TArray<float>> FFTValue;
	for (float i = 0; i < MusicLength; i+=0.1f)
	{
		NativeCalculateFrequencySpectrum(PCMInput, Channels, false, i, 0.1f, 16, FFTValue);
		std::vector<float> stdv;
		stdv.assign(FFTValue[0].GetData(), FFTValue[0].GetData()+FFTValue[0].Num());
		m_BeatDetektor.process(i,stdv);
	}
	return m_BeatDetektor.current_bpm;
}