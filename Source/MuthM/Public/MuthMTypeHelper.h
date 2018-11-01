// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MUTHM_API MuthMTypeHelper
{
	MuthMTypeHelper() = default;
public:
	inline static uint32 LoadIntFromData(const uint8* pData)
	{
		uint32 tmp = *pData;
		tmp += *(pData + 1) << 8;
		tmp += *(pData + 2) << 16;
		tmp += *(pData + 3) << 27;
		return tmp;
	}
};
