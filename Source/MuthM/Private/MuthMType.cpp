// Fill out your copyright notice in the Description page of Project Settings.

#include "MuthMType.h"
#include "FileHelper.h"
#include "LogMacros.h"
#include "MuthMTypeHelper.h"

DEFINE_LOG_CATEGORY(MDATFile)

MuthMType::MuthMType()
{
}

MuthMType::~MuthMType()
{
}

bool FMDATFile::_DeserializeInternal(const uint8* _pData)
{
	if (FMemory::Memcmp(_pData, "_MDAT", 5))
	{
		UE_LOG(MDATFile, Error, TEXT("Invalid MDAT Data"));
		return false;
	}
	const uint8* _pDataOrigin = _pData;
	while (true)
	{
		auto Convertion = FUTF8ToTCHAR((const ANSICHAR*)_pData);
		//if end of file list
		if (!Convertion.Length())
			break;
		_pData += sizeof(UTF8CHAR)*(Convertion.Length() + 1); //Include terminator char
		FString FileName = Convertion.Get();
		FileInfo CurFile;
		uint32 FileAddress = MuthMTypeHelper::LoadIntFromData(_pData);
		uint32 FileSize = MuthMTypeHelper::LoadIntFromData(_pData);
		_pData += sizeof(uint32);
		CurFile.bLoaded = false;
		CurFile.Address = FileAddress;
		CurFile.Length = FileSize;
		_Files.Add(FileName, CurFile);
	}
	_pData += sizeof(UTF8CHAR);
	_DataAddressBase = _pData - _pDataOrigin;
	return true;
}

void FMDATFile::_SerializeInternal(TArray<uint8>& DataResult)
{

}

bool FMDATFile::LoadFromFile(FString FileName)
{
	TArray<uint8> Result;
	if (!FFileHelper::LoadFileToArray(Result, *FileName))
	{
		UE_LOG(MDATFile, Error, TEXT("Load File %s Failed!"), *FileName);
		return false;
	}
	bool IsSucceed = _DeserializeInternal(Result.GetData());
	if (!IsSucceed)
		UE_LOG(MDATFile, Error, TEXT("At File Name:%s"), *FileName);
	return IsSucceed;
}
