// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ScoreEditorEntryUIBase.h"
#include "Paths.h"
#include "FileManager.h"
#include "MuthMType.h"
#include "JsonReader.h"
#include "MuthMTypeHelper.h"
#include "UserManager.h"
#include "InstructionManager.h"
#include "FileHelper.h"

FString UScoreEditorEntryUIBase::ConstructMDATPath(const FString& MDATFileName)
{
	return FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("/MDATs/"), *MDATFileName);
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
	if (!tmpMDAT->LoadFromFile(ConstructMDATPath(FileName)))
		return false;
	_MDATInstance = tmpMDAT;
	//GetFolderTree;
	pFolderTreeStack.Empty();
	pRootFolderTree = MakeShareable(new FolderTree(_MDATInstance->GenFolderTree()));
	pFolderTreeStack.Add(pRootFolderTree.Get());
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

bool UScoreEditorEntryUIBase::FolderTreeEnterFolder(const FString& FolderName)
{
	for (int i=0;i<pFolderTreeStack.Top()->Folders.Num();i++)
	{
		if (pFolderTreeStack.Top()->Folders[i].FolderName == FolderName)
		{
			pFolderTreeStack.Push(&pFolderTreeStack.Top()->Folders[i]);
			return true;
		}
	}
	//Folder not found;
	return false;
}

bool UScoreEditorEntryUIBase::FolderTreeExitFolder()
{
	if (pFolderTreeStack.Num() <= 1)
		return false; //Already in root folder.
	pFolderTreeStack.Pop();
	return true;
}

void UScoreEditorEntryUIBase::FolderTreeGetCurrent(FString& FolderName, TArray<FString>& SubFolders, TArray<FString>& Files,bool& IsRoot) const
{
	for (int ii = 0; ii < pFolderTreeStack.Top()->Folders.Num(); ii++)
		SubFolders.Add(pFolderTreeStack.Top()->Folders[ii].FolderName);
	Files = pFolderTreeStack.Top()->Files;
	FolderName = pFolderTreeStack.Top()->FolderName;
	IsRoot = pFolderTreeStack.Num() == 1;
}

FString UScoreEditorEntryUIBase::FolderTreeGetCurrentPath()
{
	FString FullPath;
	for (int i = 0; i < pFolderTreeStack.Num(); i++)
		FullPath += "/" + pFolderTreeStack[i]->FolderName;
	return FullPath;
}

bool UScoreEditorEntryUIBase::AddMMSFileToCurrentFolder(const FString& FileName)
{
	if (!_MDATInstance.IsValid())
		return false;
	FString FileFullName;
	FString TargetFileName= FileName.Replace(TEXT("/"), TEXT(""));
	for (int i=0;i<pFolderTreeStack.Num();i++)
		FileFullName += "/" + pFolderTreeStack[i]->FolderName;
	FileFullName += "/" + TargetFileName;
	if (pFolderTreeStack.Top()->Files.Find(TargetFileName)!=INDEX_NONE)
		return false;
	//Write Empty Data To MMS.
	_MDATInstance->AddFile(FileFullName, IInstructionManager::Get(this)->GenMMScript(false)->Serialize());
	pFolderTreeStack.Top()->Files.Add(TargetFileName);
	return true;
}

bool UScoreEditorEntryUIBase::AddFileToCurrentFolder(const FString& LocalFileName, const FString& TargetFileName)
{
	if (!_MDATInstance.IsValid())
		return false;
	FString tFileName=TargetFileName.Replace(TEXT("/"),TEXT(""));
	if (!IFileManager::Get().FileExists(*LocalFileName)|| !_MDATInstance.IsValid())
		return false;
	if (_MDATInstance->IsFileExist(tFileName))
		return false;
	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *tFileName))
		return false;
	return _MDATInstance->AddFile(tFileName, FileData);
}

bool UScoreEditorEntryUIBase::CreateFolderInCurrentFolder(const FString& NewFolderName)
{
	if (!_MDATInstance.IsValid())
		return false;
	for (int i = 0; i < pFolderTreeStack.Last()->Folders.Num(); i++)
	{
		if (pFolderTreeStack.Last()->Folders[i].FolderName == NewFolderName)
			return false;
	}
	FolderTree EmptyFolder;
	EmptyFolder.FolderName = NewFolderName;
	pFolderTreeStack.Last()->Folders.Add(EmptyFolder);
	return true;
}

bool UScoreEditorEntryUIBase::MoveFileInCurrentFolder(const FString& FileName, const FString& NewFileName)
{
	if (pFolderTreeStack.Last()->Files.Find(FileName) != INDEX_NONE||NewFileName.EndsWith("/")|| !_MDATInstance.IsValid())
		return false;
	FString TargetNewFileName = NewFileName;
	//TODO: May duplicated with FormatFileName().
	FPaths::RemoveDuplicateSlashes(TargetNewFileName);
	if (TargetNewFileName[0] != TEXT('/'))
		TargetNewFileName = "/" + TargetNewFileName;
	TArray<FString> FolderStack;
	FString L, R;
	while (TargetNewFileName != "")
	{
		if(!TargetNewFileName.Split("/", &L, &R))
			break;
		TargetNewFileName = R;
		FolderStack.Add(L);
	}
	FolderTree* FT = pRootFolderTree.Get();
	for (int i=1;i<FolderStack.Num();i++)
	{
		FolderTree* pTmpFolderT = FT->Folders.FindByPredicate([=](const FolderTree& a) {return a.FolderName == FolderStack[i]; });
		if (!pTmpFolderT)
		{
			FolderTree EmptyFolder;
			EmptyFolder.FolderName = FolderStack[i];
			int FolderIndex = FT->Folders.Add(EmptyFolder);
			pTmpFolderT = &FT->Folders[FolderIndex];
		}
		FT = pTmpFolderT;
	}
	//XXX: Move File may have some trouble.
	_MDATInstance->MoveFile(FolderTreeGetCurrentPath() + "/" + FileName, NewFileName);
	FT->Files.Add(TargetNewFileName);
	pFolderTreeStack.Last()->Files.Remove(FileName);
	return true;
}

bool UScoreEditorEntryUIBase::RemoveFileInFolder(const FString& FileName)
{
	if (pFolderTreeStack.Last()->Files.Find(FileName) != INDEX_NONE && !_MDATInstance.IsValid())
		return false;
	if (_MDATInstance->RemoveFile(FolderTreeGetCurrentPath() + "/" + FileName))
	{
		pFolderTreeStack.Last()->Files.Remove(FileName);
		return true;
	}
	else
		return false;
}

bool UScoreEditorEntryUIBase::RemoveFolderInFolder(const FString& FolderName)
{
	if (pFolderTreeStack.Last()->Folders.FindByPredicate([=](const FolderTree& a) {return a.FolderName==FolderName;}) && !_MDATInstance.IsValid())
		return false;
	_MDATInstance->RemoveFolder(FolderTreeGetCurrentPath() + "/" + FolderName);
	pFolderTreeStack.Last()->Folders.RemoveAll([=](const FolderTree& a) {return a.FolderName == FolderName; });
	return true;
}
