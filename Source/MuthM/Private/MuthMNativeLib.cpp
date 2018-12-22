// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMNativeLib.h"
#include "opus.h"
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"
#include "LogMacros.h"
#include "AudioResampler.h"
#include "BeatDetektor.h"
#include "tools/kiss_fftnd.h"
#include <vector>
#include "OpusAudioInfo.h"

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

/*int MP3_DebugLambda(void *user_data, const uint8_t *frame, int frame_size, size_t offset, mp3dec_frame_info_t *info)
{
	FtmpItData* user_data_it = (FtmpItData*)user_data;
	mp3dec_frame_info_t thisFrameInfo;
	mp3dec_decode_frame(user_data_it->Mp3dec, frame, frame_size, user_data_it->pFrameBuffer->GetData(), &thisFrameInfo);
	user_data_it->pOriginalPCMData->Append((uint8*)user_data_it->pFrameBuffer->GetData(), thisFrameInfo.frame_bytes);
	return 0; //Return 1 will break the decode loop
}*/

bool MuthMNativeLib::NativeDecodeMP3ToStdPCM(const TArray<uint8>& _MP3Data, TArray<uint8>& OutputStdPCM)
{
	constexpr opus_int32 samplingRate = 48000;
	OutputStdPCM.Empty();
	//Begin MP3 Decode
	mp3dec_file_info_t mp3FileInfo;
	/*struct FtmpItData
	{
		mp3dec_t* Mp3dec;
		TArray<uint8>* pOriginalPCMData;
		TArray<mp3d_sample_t>* pFrameBuffer;
		mp3dec_file_info_t* pFileInfo;
	};*/
	//FtmpItData tmpItData;
	//TArray<mp3d_sample_t> FrameBuffer;
	//FrameBuffer.SetNum(MINIMP3_MAX_SAMPLES_PER_FRAME);
	mp3dec_t MiniMp3Dec;
	mp3dec_init(&MiniMp3Dec);
	/*tmpItData.Mp3dec = &MiniMp3Dec;
	tmpItData.pOriginalPCMData = &OutputStdPCM;
	tmpItData.pFrameBuffer = &FrameBuffer;*/
	mp3dec_load_buf(&MiniMp3Dec, _MP3Data.GetData(), _MP3Data.Num(), &mp3FileInfo, nullptr, nullptr);
	//tmpItData.pFileInfo = &mp3FileInfo;
	//mp3dec_iterate_buf(_MP3Data.GetData(), _MP3Data.Num(), &MP3_DebugLambda, &tmpItData);
	//Resample
	OutputStdPCM.Append((const uint8*)mp3FileInfo.buffer, mp3FileInfo.samples * 2); //sizeof(int16_t)==2*sizeof(uint8)
	free(mp3FileInfo.buffer);
	size_t SampleDataLength = OutputStdPCM.Num();
	ResampleWaveData(OutputStdPCM, SampleDataLength, mp3FileInfo.channels, mp3FileInfo.hz, samplingRate);
	OutputStdPCM.SetNum(SampleDataLength);
	if (mp3FileInfo.channels == 1)
	{
		//Convert to 2 Channels
		int32 SampleCount = OutputStdPCM.Num();
		OutputStdPCM.SetNum(OutputStdPCM.Num() * 2);
		for (int i = SampleCount - 1; i >= 0; i -= 2)
		{
			FMemory::Memcpy(OutputStdPCM.GetData() + i, OutputStdPCM.GetData() + i * 2 + sizeof(uint16), sizeof(uint16));
			FMemory::Memcpy(OutputStdPCM.GetData() + i, OutputStdPCM.GetData() + i * 2, sizeof(uint16));
		}
	}
	return true;
	//MP3 Data Decode Finish.
}

bool MuthMNativeLib::NativeEncodeStdPCMToOpus(const TArray<uint8>& _StdPCM, TArray<uint8>& OutputOpus)
{
	int retmsg;
	TArray<uint8> _MegabyteBuffer;
	constexpr int BufferSize = 1024 * 1024;
	constexpr opus_int32 samplingRate = 48000;
	_MegabyteBuffer.SetNum(BufferSize);
	OutputOpus.Empty();
	//Begin Opus Encode
	//XXX: big little endian problem(May cause bugs in some situation?)
	OpusEncoder* _pOpusEncoder = opus_encoder_create(samplingRate, 2, OPUS_APPLICATION_AUDIO, &retmsg);
	opus_encoder_ctl(_pOpusEncoder, OPUS_SET_BITRATE(192 * 1024));
	opus_encoder_ctl(_pOpusEncoder, OPUS_SET_FORCE_CHANNELS(2));
	if (retmsg != OPUS_OK)
		return false;
	//opus_encoder_init(_pOpusEncoder, 48000, 2, OPUS_APPLICATION_AUDIO);
	//Use 480 Sample per decode
	//Will decode more than 18000 times for every sound.
	constexpr int PerDecodeSampleNum = 480;
	int PerDecodeSize = PerDecodeSampleNum * 2 * sizeof(uint16); //SampleNum*ChannelCount(2)*16Bit
	int EncodedOpusSize;
	int HeaderIndex = FCStringAnsi::Strlen(OPUS_ID_STRING);
	OutputOpus.SetNum(HeaderIndex + 73); //Terminal Character and opus info data,See OpusAudioInfo.cpp:ParseHeader
	FMemory::Memcpy(OutputOpus.GetData(), OPUS_ID_STRING,HeaderIndex+1);
	HeaderIndex++;
	uint32 SampleCount=0;
	for (int i = 0; i < _StdPCM.Num(); i += PerDecodeSize)
	{
		EncodedOpusSize = opus_encode(_pOpusEncoder,
			(const opus_int16*)(_StdPCM.GetData() + i),
			PerDecodeSampleNum, _MegabyteBuffer.GetData(),
			_MegabyteBuffer.Num());
		OutputOpus.Append(_MegabyteBuffer.GetData(), EncodedOpusSize);
		SampleCount++;
	}
	*(uint16*)(OutputOpus.GetData() + HeaderIndex) = 48000;  //Sample rate.
	HeaderIndex += sizeof(uint16);
	*(uint32*)(OutputOpus.GetData() + HeaderIndex) = _StdPCM.Num()/4; //True sample count(Samples count /16bit/2channels).
	HeaderIndex += sizeof(uint32);
	*(uint8*)(OutputOpus.GetData() + HeaderIndex) = 2; //Num Channels
	HeaderIndex += sizeof(uint8);
	*(uint32*)(OutputOpus.GetData() + HeaderIndex) = SampleCount; //Frame Count
	return true;
	//End Opus Encode
}

void MuthMNativeLib::NativeCalculateFrequencySpectrum(const TArray<uint8>& _StdPCMInput, bool SplitChannels, const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<TArray<float>>& OutSpectrums)
{
	OutSpectrums.Empty();
	constexpr int SampleRate = 48000;
	int BeginSampleIndex = BeginTime * SampleRate;
	int EndSampleIndex = (BeginTime + SampleTimeLength)*SampleRate;
	int SampleCount = EndSampleIndex - BeginSampleIndex;
	TArray<int16> StdSampleData;
	StdSampleData.SetNum(SampleCount * 2);
	FMemory::Memcpy(StdSampleData.GetData(), _StdPCMInput.GetData() + BeginSampleIndex, SampleCount * sizeof(uint16) * 2);
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
		for (int cc = 0; cc < ChannelCount; cc++)
		{
			inbuf[cc][i].r = _FFTHannWindow(StdSampleData[i], i, SampleCount);
			inbuf[cc][i].i = 0.f;
		}
	}
	OutSpectrums.SetNum(ChannelCount);
	for (int cc = 0; cc < ChannelCount; cc++)
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

float MuthMNativeLib::NativeDetectBPMFromPCM(const TArray<uint8>& _StdPCMInput)
{
	constexpr int StdSampleRate = 48000;
	//Because <³õÒô¥ß¥¯¤ÎÏûÊ§> is far beyond 200BPM,so set a higher value
	//Maybe is not necessary.
	BeatDetektor m_BeatDetektor(100.f,300.f);
	m_BeatDetektor.reset();
	float MusicLength = int(_StdPCMInput.Num() / StdSampleRate);
	TArray<TArray<float>> FFTValue;
	for (float i = 0; i < MusicLength; i+=0.1f)
	{
		NativeCalculateFrequencySpectrum(_StdPCMInput, false, i, 0.1f, 16, FFTValue);
		std::vector<float> stdv;
		stdv.assign(FFTValue[0].GetData(), FFTValue[0].GetData()+FFTValue[0].Num());
		m_BeatDetektor.process(i,stdv);
	}
	return m_BeatDetektor.current_bpm;
}