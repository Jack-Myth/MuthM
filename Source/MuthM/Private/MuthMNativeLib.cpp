// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMNativeLib.h"
#include "libmpg123/mpg123.h"
#include "opus.h"

TArray<uint8> MuthMNativeLib::NativeDecodeMP3ToOpus(const TArray<uint8>& _MP3Data)
{
	//TODO: Performance Improvement request.
	//It doesn't need to decode the whole MP3 to PCM, maybe some sample and encode to Opus immediately
	//It can reduce some memory usage.
	mpg123_init();
	int retmsg;
	TArray<uint8> _PCMData;
	TArray<uint8> _MegabyteBuffer;
	constexpr int BufferSize = 1024 * 1024;
	_MegabyteBuffer.SetNum(BufferSize);
	//Begin MP3 Decode
	mpg123_handle* _decoderHandle = mpg123_new(nullptr, &retmsg);
	mpg123_open_feed(_decoderHandle);
	mpg123_format_none(_decoderHandle);
	mpg123_format(_decoderHandle, 48000, MPG123_STEREO, MPG123_ENC_16);
	mpg123_param(_decoderHandle, MPG123_FORCE_RATE, 48000, 0);
	size_t OutSize;
	retmsg = mpg123_decode(_decoderHandle, _MP3Data.GetData(), _MP3Data.Num(), _MegabyteBuffer.GetData(), BufferSize, &OutSize);
	while (retmsg==MPG123_NEED_MORE)
	{
		_PCMData.Append(_MegabyteBuffer);
		retmsg = mpg123_decode(_decoderHandle,NULL,0, _MegabyteBuffer.GetData(), BufferSize, &OutSize);
	}
	if (retmsg == MPG123_ERR)
		return TArray<uint8>();
	_PCMData.Append(_MegabyteBuffer.GetData(),OutSize);
	mpg123_close(_decoderHandle);
	mpg123_delete(_decoderHandle);
	mpg123_exit();
	//MP3 Data Decode Finish.
	//Begin Opus Encode
	//XXX: big little endian problem(May cause bugs in some situation?)
	opus_int32 samplingRate = 48000;
	OpusEncoder* _pOpusEncoder = opus_encoder_create(samplingRate, 2, OPUS_APPLICATION_AUDIO, &retmsg);
	opus_encoder_ctl(_pOpusEncoder, OPUS_SET_BITRATE(192 * 1024));
	opus_encoder_ctl(_pOpusEncoder, OPUS_SET_FORCE_CHANNELS(2));
	if (retmsg != OPUS_OK)
		return TArray<uint8>();
	//opus_encoder_init(_pOpusEncoder, 48000, 2, OPUS_APPLICATION_AUDIO);
	//Use 480 Sample per decode
	//Will decode more than 18000 times for every sound.
	constexpr int PerDecodeSampleNum = 480;
	constexpr int PerDecodeSize = PerDecodeSampleNum * 2 * 2; //SampleNum*ChannelCount(2)*16Bit(2)
	int EncodedOpusSize;
	TArray<uint8> OpusData;
	for (int i=0;i<_PCMData.Num();i+= PerDecodeSize)
	{
		EncodedOpusSize=opus_encode(_pOpusEncoder, 
			(const opus_int16*)(_PCMData.GetData() + i),
			PerDecodeSampleNum, _MegabyteBuffer.GetData(), 
			_MegabyteBuffer.Num());
		OpusData.Append(_MegabyteBuffer.GetData(), EncodedOpusSize);
	}
	return OpusData;
	//End Opus Encode
}
