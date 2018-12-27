// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ScoreEditorEntryUIBase.h"
#include "Paths.h"
#include "FileManager.h"
#include "MuthMType.h"
#include "JsonReader.h"
#include "MuthMTypeHelper.h"
#include "UserManager.h"

FString UScoreEditorEntryUIBase::ConstructMDATPath(const FString& MDATFileName)
{
	return *FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("/MDATs/"), *MDATFileName);
}

void UScoreEditorEntryUIBase::GetSuitableMDATs(TArray<FString>& MDATFileName) const
{
	IFileManager::Get().FindFiles(MDATFileName, *FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("/MDATs/")));
	for (int i=0;i< MDATFileName.Num();i++)
		MDATFileName[i] = FPaths::GetCleanFilename(MDATFileName[i]);
}

bool UScoreEditorEntryUIBase::CreateNewMDAT(const FString& FileName)
{
	FString TargetFileName = ConstructMDATPath(FileName);
	if (IFileManager::Get().FileExists(*TargetFileName))
		return false;
	FMDATFile CurMDAT;
	return CurMDAT.Save(TargetFileName);
}

void UScoreEditorEntryUIBase::UpdateMDATInfo(const FString& FileName, FMDATMainInfo MDATInfo)
{
	FMDATFile tmpMDAT;
	if (!tmpMDAT.LoadFromFile(ConstructMDATPath(FileName)))
		return;
	FMDATMainInfo MainInfo;
	if (!MuthMTypeHelper::TryGetMDATMainInfo(&tmpMDAT, MainInfo))
		return;
	MainInfo.Title = MDATInfo.Title;
	MainInfo.Description = MDATInfo.Description;
	MainInfo.AuthorID = IUserManager::Get(this)->IsLoggedIn() ? IUserManager::Get(this)->GetUserID() : -1;
	MainInfo.AuthorName = MDATInfo.AuthorName;
	MainInfo.Cover = MDATInfo.Cover;
	MuthMTypeHelper::SaveMDATMainInfo(&tmpMDAT, MainInfo);
}

bool UScoreEditorEntryUIBase::OpenMDAT(const FString& FileName, FMDATMainInfo& MDATInfo)
{
	TSharedPtr<FMDATFile> tmpMDAT = MakeShareable(new FMDATFile());
	if (!tmpMDAT->LoadFromFile(FileName))
		return false;
	_MDATInstance = tmpMDAT;
	return MuthMTypeHelper::TryGetMDATMainInfo(_MDATInstance.Get(), MDATInfo);
}

bool UScoreEditorEntryUIBase::GetFileFromOpenedMDAT(const FString& FileName, TArray<uint8>& FileData) const
{
	if (!_MDATInstance.IsValid())
		return false;
	if (_MDATInstance->IsFileExist(FileName))
	{
		FileData = _MDATInstance->GetFileData(FileName);
		return true;
	}
	else
		return false;
}

TArray<FString> UScoreEditorEntryUIBase::GetOpenedMDATFileList()
{
	if (!_MDATInstance.IsValid())
		return TArray<FString>();
	return _MDATInstance->GetAllFileNames();
}

void UScoreEditorEntryUIBase::SaveOpenedMDATMainInfo(const FMDATMainInfo& MDATInfo)
{
	if (!_MDATInstance.IsValid())
		return;
	MuthMTypeHelper::SaveMDATMainInfo(_MDATInstance.Get(),MDATInfo);
}

bool UScoreEditorEntryUIBase::RenameOpenedMDATFile(const FString& NewFileName)
{
	if (IFileManager::Get().FileExists(*ConstructMDATPath(NewFileName)))
		return false;
	FString OpenedFileName = _MDATInstance->GetLocalFileName();
	return IFileManager::Get().Move(*ConstructMDATPath(NewFileName), *OpenedFileName,false);
}

void UScoreEditorEntryUIBase::DeleteOpenedMDATFile()
{
	if (!_MDATInstance.IsValid())
		return;
	FString MDATFileName = _MDATInstance->GetLocalFileName();
	_MDATInstance.Reset();
	IFileManager::Get().Delete(*MDATFileName);
}
