// Fill out your copyright notice in the Description page of Project Settings.

#include "MuthMType.h"
#include "FileHelper.h"
#include "LogMacros.h"
#include "MuthMTypeHelper.h"
#include "zlib.h"
#include "PlatformFilemanager.h"

DEFINE_LOG_CATEGORY(MDATFileLog)

bool FMDATFile::_DeserializeInternal(const uint8* _pData)
{
	if (FMemory::Memcmp(_pData, "_MDAT", 5))
	{
		UE_LOG(MDATFileLog, Error, TEXT("Invalid MDAT Data"));
		return false;
	}
	const uint8* _pDataOrigin = _pData;
	_pData += 5; //Length of "_MDAT"
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
		_pData += sizeof(uint32);
		uint32 FileSize = MuthMTypeHelper::LoadIntFromData(_pData);
		_pData += sizeof(uint32);
		uint32 OriginalSize = MuthMTypeHelper::LoadIntFromData(_pData);
		_pData += sizeof(uint32);
		CurFile.bLoaded = true;
		CurFile.Address = FileAddress;
		CurFile.CompressedLength = FileSize;
		CurFile.OriginalLength = OriginalSize;
		_Files.Add(FileName, CurFile);
	}
	_pData += sizeof(UTF8CHAR);
	_DataAddressBase = _pData - _pDataOrigin;
	for (auto it=_Files.CreateIterator();it;++it)
	{
		it->Value.Data.SetNum(it->Value.OriginalLength);
		unsigned long FileLength = it->Value.OriginalLength;
		uncompress(it->Value.Data.GetData(), &FileLength, _pData + it->Value.Address, it->Value.CompressedLength);
		it->Value.bLoaded = true;
	}
	return true;
}

bool FMDATFile::_DeserializeInternal_Lazy(IFileHandle* FileHandle)
{
	TArray<uint8> tmpData;
	tmpData.SetNum(5);
	FileHandle->Seek(0);
	FileHandle->Read(tmpData.GetData(), 5);
	if (FMemory::Memcmp(tmpData.GetData(), "_MDAT", 5))
	{
		UE_LOG(MDATFileLog, Error, TEXT("Invalid MDAT Data"));
		return false;
	}
	while (true)
	{
		tmpData.SetNum(0, false);
		ANSICHAR CurChar;
		do
		{
			FileHandle->Read((uint8*)&CurChar, sizeof(ANSICHAR));
			tmpData.Push(CurChar);
		} while (CurChar);
		//if end of file list
		if (!tmpData[0])
			break;
		auto Convertion = FUTF8ToTCHAR((const ANSICHAR*)tmpData.GetData());
		FString FileName = Convertion.Get();
		FString SimplestFileName = FormatFileName(FileName);
#if !UE_BUILD_SHIPPING
		if (FileName!=SimplestFileName)
			UE_LOG(MDATFileLog,Warning,TEXT("FileName \"%s\" is not the samplest."),*FileName)
#endif
		FileInfo CurFile;
		tmpData.SetNum(sizeof(uint32),false);
		FileHandle->Read(tmpData.GetData(), sizeof(uint32));
		uint32 FileAddress = MuthMTypeHelper::LoadIntFromData(tmpData.GetData());
		FileHandle->Read(tmpData.GetData(), sizeof(uint32));
		uint32 FileSize = MuthMTypeHelper::LoadIntFromData(tmpData.GetData());
		FileHandle->Read(tmpData.GetData(), sizeof(uint32));
		uint32 OriginalFileLength = MuthMTypeHelper::LoadIntFromData(tmpData.GetData());
		CurFile.bLoaded = false;
		CurFile.Address = FileAddress;
		CurFile.CompressedLength = FileSize;
		CurFile.OriginalLength = OriginalFileLength;
		_Files.Add(SimplestFileName, CurFile);
	}
	_DataAddressBase = FileHandle->Tell();
	return true;
}

void FMDATFile::_SerializeInternal(TArray<uint8>& DataResult)
{
	DataResult.SetNum(0);
	DataResult.Append((const uint8*)"_MDAT", 5);
	TArray<uint8> DataArea;
	TArray<uint8> CompressBuffer;
	for (auto it=_Files.CreateIterator();it;++it)
	{
		if (!it->Value.bLoaded)
			_LazyLoad(&(it->Value));
		it->Value.Address = DataArea.Num();
		FTCHARToUTF8 TTU(*it->Key);
		CompressBuffer.SetNum(it->Value.CompressedLength ? it->Value.CompressedLength : it->Value.OriginalLength);
		unsigned long DestLen = CompressBuffer.Num();
		compress2(CompressBuffer.GetData(), &DestLen, it->Value.Data.GetData(), it->Value.Data.Num(), Z_BEST_COMPRESSION);
		it->Value.CompressedLength = DestLen;
		DataArea.Append(CompressBuffer.GetData(), DestLen);
		DataResult.Append((const uint8*)TTU.Get(), TTU.Length()+1); //Include Terminator Character
		DataResult.Append(MuthMTypeHelper::SaveIntToData(it->Value.Address));
		DataResult.Append(MuthMTypeHelper::SaveIntToData(DestLen));
		DataResult.Append(MuthMTypeHelper::SaveIntToData(it->Value.OriginalLength));
	}
	DataResult.Add(0);
	DataResult.Append(DataArea);
}

void FMDATFile::_LazyLoad(FileInfo* pFileInfo) const
{
	if (pFileInfo->bLoaded)
		return;
	IFileHandle* FileHandle = FPlatformFileManager::Get().GetPlatformFile().OpenRead(*_MDATFileName);
	if (!FileHandle)
	{
		UE_LOG(MDATFileLog, Error, TEXT("Unable to load data,Open MDAT File(%s) Failed!"), *_MDATFileName);
		return;
	}
	FileHandle->Seek(pFileInfo->Address + _DataAddressBase);
	TArray<uint8> CompressedData;
	CompressedData.SetNum(pFileInfo->CompressedLength);
	if (!FileHandle->Read(CompressedData.GetData(), pFileInfo->CompressedLength))
	{
		//Load Failed!
		UE_LOG(MDATFileLog, Error, TEXT("Unable to load data from %s!"), *_MDATFileName);
		delete FileHandle;
		return;
	}
	delete FileHandle;
	pFileInfo->Data.SetNum(pFileInfo->OriginalLength);
	unsigned long DestLen = pFileInfo->Data.Num();
	uncompress(pFileInfo->Data.GetData(), &DestLen, CompressedData.GetData(), CompressedData.Num());
	pFileInfo->bLoaded = true;
}

FString FMDATFile::FormatFileName(const FString& FileName)
{
	FString TargetFilePath = FileName;
	TargetFilePath.Replace(TEXT("\\"), TEXT("/"));
	//Clear Duplicated '/'
	for (auto it = TargetFilePath.CreateIterator(); it; ++it)
	{
		if (*it == TEXT('/'))
		{
			for (auto itx = it + 1; itx; ++itx)
			{
				if (*itx == TEXT('/'))
					itx.RemoveCurrent();
				else
					break;
			}
		}
	}
	if (TargetFilePath==""||TargetFilePath[0] != TEXT('/'))
		TargetFilePath = TEXT("/")+TargetFilePath;
	return TargetFilePath;
}

FolderTree FMDATFile::_GenFolderTreeInternal(const TArray<FString>& FileNames, const FString& FolderName)
{
	//TODO:Performance Warning.
	TMap<FString, TArray<FString>> FolderNames;
	FolderTree FT;
	for (int i=0;i<FileNames.Num();i++)
	{
		FString L, R;
		if (!FileNames[i].Split("/", &L, &R))
		{
			//Is File,can not be split.
			FT.Files.Add(FileNames[i]);
		}
		else
		{
			//Have Folder.
			FolderNames.FindOrAdd(L).Add(R);
		}
	}
	for (auto it=FolderNames.CreateIterator();it;++it)
	{
		FT.Folders.Add(_GenFolderTreeInternal(it->Value, it->Key));
	}
	FT.FolderName = FolderName;
	return FT;
}

bool FMDATFile::LoadFromFile(FString FileName)
{
	TArray<uint8> Result;
	IFileHandle* FileHandle = FPlatformFileManager::Get().GetPlatformFile().OpenRead(*FileName);
	if (!FileHandle)
	{
		UE_LOG(MDATFileLog, Error, TEXT("Load File %s Failed!"), *FileName);
		return false;
	}
	bool IsSucceed = _DeserializeInternal_Lazy(FileHandle);
	delete FileHandle; //Close File
	if (!IsSucceed)
	{
		UE_LOG(MDATFileLog, Error, TEXT("At File Name:%s"), *FileName);
	}
	else
	{
		_MDATFileName = FileName;
	}
	return IsSucceed;
}

TArray<uint8> FMDATFile::GetFileData(FString FileName) const
{
	FileName = FormatFileName(FileName);
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
	FileName = FormatFileName(FileName);
	if (FileName.EndsWith("/"))
		return false;
	if (IsFileExist(FileName))
		return false;
	FileInfo& tmpFileInfo= _Files.Add(FileName);
	tmpFileInfo.bLoaded = true;
	tmpFileInfo.Data = FileData;
	tmpFileInfo.Address = 0;
	tmpFileInfo.OriginalLength = FileData.Num();
	return true;
}

bool FMDATFile::Save(FString FileName)
{
	if (FileName == "" && _MDATFileName == "")
		return false;
	FString TargetSaveFile = FileName != "" ? *FileName : *_MDATFileName;
	TArray<uint8> ResData;
	Serialize(ResData);
	//if (TargetSaveFile==_MDATFileName)
		//IFileManager::Get().Delete(*TargetSaveFile);
	return FFileHelper::SaveArrayToFile(ResData, *TargetSaveFile);
}

TArray<FString> FMDATFile::GetAllFileNames()
{
	TArray<FString> FileNames;
	_Files.GetKeys(FileNames);
	return FileNames;
}

void FMDATFile::MakeEmpty()
{
	_Files.Empty();
	_MDATFileName = "";
	_DataAddressBase = 0;
}

FolderTree FMDATFile::GenFolderTree()
{
	TArray<FString> FileNames;
	_Files.GetKeys(FileNames);
	FolderTree FT = _GenFolderTreeInternal(FileNames, "/");
	if (FT.Folders.Num())
	{
		//All folder is start with "/"
		//so the real FolderTree must inside FT.
		FT.Folders[0].FolderName = "/";
		return FT.Folders[0];
	}
	else
		return FolderTree();
}

bool FMDATFile::MoveFile(const FString& OriginalFileName, const FString& TargetFileName)
{
	FString _OriFileName = FormatFileName(OriginalFileName);
	FString _TargetFileName = FormatFileName(TargetFileName);
	if (!_Files.Find(_OriFileName)||_Files.Find(_TargetFileName))
		return false;
	FileInfo FI;
	_Files.RemoveAndCopyValue(_OriFileName, FI);
	_Files.Add(_TargetFileName, FI);
	return true;
}

bool FMDATFile::RemoveFolder(const FString& FolderFullName)
{
	FString TargetFolderName = FormatFileName(FolderFullName);
	if (!TargetFolderName.EndsWith("/"))
		TargetFolderName += "/";
	bool IsAnyRemoved=false;
	for (auto it=_Files.CreateIterator();it;++it)
	{
		if (it->Key.StartsWith(TargetFolderName))
		{
			it.RemoveCurrent();
			IsAnyRemoved = true;
		}
	}
	return IsAnyRemoved;
}
