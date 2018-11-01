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
		uint32 tmp = *(pData + 3);
		tmp = tmp << 8 + *(pData + 2);
		tmp = tmp << 8 + *(pData + 1);
		tmp = tmp << 8 + *pData;
		return tmp;
	}
};
