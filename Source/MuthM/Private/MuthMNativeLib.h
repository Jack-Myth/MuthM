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
	TArray<uint8> NativeDecodeMP3ToOpus(const TArray<uint8>& _MP3Data);
	
};
