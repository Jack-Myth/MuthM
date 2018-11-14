// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MuthMNativeLib
{
public:
	TArray<uint8> NativeEncodePCMToOpus(const TArray<uint8>& _PCMData);
};
