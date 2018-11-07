// Copyright (C) 2018 JackMyth. All Rights Reserved.

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
		tmp = (tmp << 8) + *(pData + 2);
		tmp = (tmp << 8) + *(pData + 1);
		tmp = (tmp << 8) + *pData;
		return tmp;
	}

	inline static float LoadFloatFromData(const uint8* pData)
	{
		uint32 tmpIntData = LoadIntFromData(pData);
		auto* floatData = (float*)&tmpIntData;
		return *floatData;
	}

	inline static TArray<uint8> SaveIntToData(uint32 Int)
	{
		TArray<uint8> DataToSave;
		DataToSave.Add(0x000000FF | Int);
		Int >>= 8;
		DataToSave.Add(0x000000FF | Int);
		Int >>= 8;
		DataToSave.Add(0x000000FF | Int);
		Int >>= 8;
		DataToSave.Add(Int);
		return DataToSave;
	}
};
