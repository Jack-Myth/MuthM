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
	static bool NativeConvertMP3ToOpus(const TArray<uint8>& _MP3Data,TArray<uint8>& OpusOutput);
	
};
