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

	static bool NativeDecodeMP3ToStdPCM(const TArray<uint8>& _MP3Data, TArray<uint8>& OutputStdPCM);
	static bool NativeEncodeStdPCMToOpus(const TArray<uint8>& _StdPCM, TArray<uint8>& OutputOpus);
	
};
