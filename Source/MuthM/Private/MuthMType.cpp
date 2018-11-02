// Fill out your copyright notice in the Description page of Project Settings.

#include "MuthMType.h"
#include "FileHelper.h"
#include "LogMacros.h"
#include "MuthMTypeHelper.h"
#include "PlatformFilemanager.h"

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
	DataResult.SetNum(0);
	DataResult.Append((const uint8*)"_MDAT", 5);
	TArray<uint8> DataArea;
	for (auto it=_Files.CreateIterator();it;++it)
	{
		if (!it->Value.bLoaded)
			_LazyLoad(&(it->Value));
		it->Value.Address = DataArea.Num();
		DataArea.Append(it->Value.Data);
		FTCHARToUTF8 TTU(*it->Key);
		DataResult.Append((const uint8*)TTU.Get(), TTU.Length()+1); //Include Terminator Character
		DataResult.Append(MuthMTypeHelper::SaveIntToData(it->Value.Address));
		DataResult.Append(MuthMTypeHelper::SaveIntToData(it->Value.Length));
	}
	DataResult.Append(DataArea);
}

void FMDATFile::_LazyLoad(FileInfo* pFileInfo) const
{
	if (pFileInfo->bLoaded)
		return;
	IFileHandle* FileHandle = FPlatformFileManager::Get().GetPlatformFile().OpenRead(*_MDATFileName);
	if (!FileHandle)
	{
		UE_LOG(MDATFile, Error, TEXT("Unable to load data,Open MDAT File(%s) Failed!"), *_MDATFileName);
		return;
	}
	FileHandle->Seek(pFileInfo->Address + _DataAddressBase);
	pFileInfo->Data.SetNum(pFileInfo->Length);
	if (!FileHandle->Read(pFileInfo->Data.GetData(), pFileInfo->Length))
	{
		//Load Failed!
		UE_LOG(MDATFile, Error, TEXT("Unable to load data from %s!"), *_MDATFileName);
		pFileInfo->Data.SetNum(0);
		return;
	}
	pFileInfo->bLoaded = true;
}

bool FMDATFile::LoadFromFile(FString FileName)
{
	TArray<uint8> Result;
	if (!FFileHelper::LoadFileToArray(Result, *FileName))
	{
		UE_LOG(MDATFile, Error, TEXT("Load File %s Failed!"), *FileName);
		return false;
	}
	bool IsSucceed = Load(Result.GetData());
	if (!IsSucceed)
		UE_LOG(MDATFile, Error, TEXT("At File Name:%s"), *FileName);
	return IsSucceed;
}

TArray<uint8> FMDATFile::GetFileData(FString FileName) const
{
	const FileInfo* TargetFileInfo = _Files.Find(FileName);
	if (TargetFileInfo)
	{
		if (!TargetFileInfo->bLoaded)
			_LazyLoad(const_cast<FileInfo*>(TargetFileInfo));
		return TargetFileInfo->Data;
	}
	return TArray<uint8>();
}

bool FMDATFile::AddFile(FString FileName, const TArray<uint8>& FileData)
{
	if (IsFileExist(FileName))
		return false;
	FileInfo& tmpFileInfo= _Files.Add(FileName);
	tmpFileInfo.bLoaded = true;
	tmpFileInfo.Data = FileData;
	tmpFileInfo.Address = 0;
	tmpFileInfo.Length = FileData.Num();
	return true;
}

bool FMDATFile::Save(FString FileName)
{
	TArray<uint8> ResData;
	Serialize(ResData);
	return FFileHelper::SaveArrayToFile(ResData, *FileName);
}