// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMNativeLib.h"
#include "opus.h"
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"
#include "LogMacros.h"
#include "AudioResampler.h"

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

bool MuthMNativeLib::NativeDecodeMP3ToStdPCM(const TArray<uint8>& _MP3Data, TArray<uint8>& OutputStdPCM)
{
	constexpr opus_int32 samplingRate = 48000;
	OutputStdPCM.Empty();
	//Begin MP3 Decode
	mp3dec_file_info_t mp3FileInfo;
	struct FtmpItData
	{
		mp3dec_t* Mp3dec;
		TArray<uint8>* pOriginalPCMData;
		TArray<mp3d_sample_t>* pFrameBuffer;
		mp3dec_file_info_t* pFileInfo;
	};
	FtmpItData tmpItData;
	TArray<mp3d_sample_t> FrameBuffer;
	FrameBuffer.SetNum(MINIMP3_MAX_SAMPLES_PER_FRAME);
	mp3dec_t MiniMp3Dec;
	mp3dec_init(&MiniMp3Dec);
	tmpItData.Mp3dec = &MiniMp3Dec;
	tmpItData.pOriginalPCMData = &OutputStdPCM;
	tmpItData.pFrameBuffer = &FrameBuffer;
	mp3dec_load_buf(&MiniMp3Dec, _MP3Data.GetData(), _MP3Data.Num(), &mp3FileInfo, nullptr, nullptr);
	tmpItData.pFileInfo = &mp3FileInfo;
	mp3dec_iterate_buf(_MP3Data.GetData(), _MP3Data.Num(),
		[](void *user_data, const uint8_t *frame, int frame_size, size_t offset, mp3dec_frame_info_t *info)->int
		{
			FtmpItData* user_data_it = (FtmpItData*)user_data;
			mp3dec_frame_info_t thisFrameInfo;
			mp3dec_decode_frame(user_data_it->Mp3dec, frame, frame_size, user_data_it->pFrameBuffer->GetData(), &thisFrameInfo);
			user_data_it->pOriginalPCMData->Append((uint8*)user_data_it->pFrameBuffer->GetData(), thisFrameInfo.frame_bytes);
			return 1;
		}, nullptr);
	//Resample
	size_t SampleDataLength = OutputStdPCM.Num();
	ResampleWaveData(OutputStdPCM, SampleDataLength, mp3FileInfo.channels, mp3FileInfo.samples, samplingRate);
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
	for (int i = 0; i < _StdPCM.Num(); i += PerDecodeSize)
	{
		EncodedOpusSize = opus_encode(_pOpusEncoder,
			(const opus_int16*)(_StdPCM.GetData() + i),
			PerDecodeSampleNum, _MegabyteBuffer.GetData(),
			_MegabyteBuffer.Num());
		OutputOpus.Append(_MegabyteBuffer.GetData(), EncodedOpusSize);
	}
	return true;
	//End Opus Encode
}
