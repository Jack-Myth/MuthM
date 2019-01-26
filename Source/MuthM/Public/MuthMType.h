// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FolderTree
{
	FString FolderName;
	TArray<FolderTree> Folders;
	TArray<FString> Files;
};

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(MDATFileLog, Log, All);
class MUTHM_API FMDATFile
{
	struct FileInfo
	{
		/*File Meta*/
		//File Address in MDAT
		uint32 Address;
		uint32 CompressedLength;
		uint32 OriginalLength;
		/*File Data*/
		TArray<uint8> Data;
		bool bLoaded : 1;
	};
	//Need To convert the File Path to the simplest
	TMap<FString, FileInfo> _Files;
	uint32 _DataAddressBase;

	//The Local File Name
	FString _MDATFileName;
	bool _DeserializeInternal(const uint8* _pData);
	bool _DeserializeInternal_Lazy(IFileHandle* FileHandle);
	void _SerializeInternal(TArray<uint8>& DataResult);
	void _LazyLoad(FileInfo* pFileInfo) const;
	FolderTree _GenFolderTreeInternal(const TArray<FString>& FileNames, const FString& FolderName);
public:
	static FString FormatFileName(const FString& FileName);

	FMDATFile()=default;

	bool LoadFromFile(const FString& FileName);

	FORCEINLINE bool Load(const TArray<uint8>& Data)
	{
		return _DeserializeInternal(Data.GetData());
	}
	FORCEINLINE bool Load(const uint8* pData)
	{
		return _DeserializeInternal(pData);
	}

	inline void LoadAllFile()
	{
		for (auto it=_Files.CreateIterator();it;++it)
		{
			if (!it->Value.bLoaded)
				_LazyLoad(&it->Value);
		}
	}

	//Get File Data By name
	//If No such file,TArray<uint8>() is returned.
	TArray<uint8> GetFileData(FString FileName) const;
	FORCEINLINE uint32 GetFileAddress(FString FileName) const
	{
		const FileInfo* TargetFileInfo = _Files.Find(FormatFileName(FileName));
		return TargetFileInfo ? TargetFileInfo->Address : (uint32)NULL;
	}
	FORCEINLINE uint32 GetFileCompressedLength(FString FileName) const
	{
		const FileInfo* TargetFileInfo = _Files.Find(FormatFileName(FileName));
		return TargetFileInfo ? TargetFileInfo->CompressedLength : (uint32)NULL;
	}
	FORCEINLINE bool IsFileExist(FString FileName) const
	{
		return _Files.Find(FormatFileName(FileName));
	}

	bool AddFile(FString FileName, const TArray<uint8>& FileData);
	FORCEINLINE bool RemoveFile(FString FileName)
	{
		return _Files.Remove(FormatFileName(FileName));
	}

	//Save MDAT to local file.
	bool Save(FString FileName="");
	FORCEINLINE void Serialize(TArray<uint8>& DataResult)
	{
		_SerializeInternal(DataResult);
	}

	TArray<FString> GetAllFileNames();

	//Clear all data restore to default mode.
	//Same as =FMDATFile();
	void MakeEmpty();

	//Return local file that the instance is open form.
	FORCEINLINE FString GetLocalFileName() const
	{
		return _MDATFileName;
	}

	FolderTree GenFolderTree();

	bool MoveFile(const FString& OriginalFileName,const FString& TargetFileName);
	bool RemoveFolder(const FString& FolderFullName);
};