// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class MUTHM_API MuthMType
{
public:
	MuthMType();
	~MuthMType();
};

DECLARE_LOG_CATEGORY_EXTERN(MDATFile, Log, All);
class MUTHM_API FMDATFile
{
	struct FileInfo
	{
		/*File Meta*/
		//File Address in MDAT
		uint32 Address;
		uint32 Length;
		/*File Data*/
		TArray<uint8> Data;
		bool bLoaded : 1;
	};
	TMap<FString, FileInfo> _Files;
	uint32 _DataAddressBase;
	bool _DeserializeInternal(const uint8* _pData);
	void _SerializeInternal(TArray<uint8>& DataResult);
	void _LazyLoad(FString FileName);
public:
	FMDATFile()=default;

	bool LoadFromFile(FString FileName);

	FORCEINLINE bool Load(const TArray<uint8>& Data)
	{
		return _DeserializeInternal(Data.GetData());
	}
	FORCEINLINE bool Load(const uint8* pData)
	{
		return _DeserializeInternal(pData);
	}

	//Get File Data By name
	//If No such file,TArray<uint8>() is returned.
	const TArray<uint8>& GetFileData(FString FileName) const;
	const uint32 GetFileAddress(FString FileName) const;
	const uint32 GetFileLength(FString FileName) const;
	const bool IsFileExist(FString FileName) const;

	bool AddFile(FString FileName, const TArray<uint8>& FileData);
	bool RemoveFile(FString FileName);
	bool Save(FString FileName);
	FORCEINLINE void Serialize(TArray<uint8>& DataResult)
	{
		_SerializeInternal(DataResult);
	}
};