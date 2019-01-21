// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MuthMBPLib.h"



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
		if (FGenericPlatformProperties::IsLittleEndian())
		{
			return *(const float*)pData;
		}
		else
		{
			float tmp;
			auto* ptmp = (uint8*)&tmp;
			*ptmp = *(pData + 3);
			*(ptmp + 1) = *(pData + 2);
			*(ptmp + 2) = *(pData + 1);
			*(ptmp + 3) = *pData;
			return tmp;
		}
	}

	inline static TArray<uint8> SaveFloatToData(const float& Float)
	{
		TArray<uint8> tmpArray;
		if (FGenericPlatformProperties::IsLittleEndian())
		{
			tmpArray.Append((uint8*)&Float, sizeof(float));
			return tmpArray;
		}
		else
		{
			tmpArray.SetNum(sizeof(float));
			auto* ptmp = (const uint8*)&Float;
			tmpArray[0] = *(ptmp + 3);
			tmpArray[1] = *(ptmp + 2);
			tmpArray[2] = *(ptmp + 1);
			tmpArray[3] = *ptmp;
			return tmpArray;
		}
	}

	inline static TArray<uint8> SaveIntToData(const uint32& Int)
	{
		TArray<uint8> DataToSave;
		DataToSave.Add(0x000000FF & Int);
		DataToSave.Add(0x000000FF & (Int >> 8));
		DataToSave.Add(0x000000FF & (Int >> 16));
		DataToSave.Add(0x000000FF & (Int >> 24));
		return DataToSave;
	}

	static bool TryGetMDATMainInfo(class FMDATFile* MDATFile, FMDATMainInfo& MainInfo);
	static void SaveMDATMainInfo(class FMDATFile* MDATFile, const FMDATMainInfo& MainInfo);
};