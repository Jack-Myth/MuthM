// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ScoreEditorEntry.h"
#include "Paths.h"
#include "FileManager.h"
#include "MuthMType.h"
#include "JsonReader.h"
#include "MuthMTypeHelper.h"
#include "UserManager.h"

FString UScoreEditorEntry::ConstructMDATPath(const FString& MDATFileName)
{
	return *FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("/MDATs/"), *MDATFileName);
}

void UScoreEditorEntry::GetSuitableMDATs(TArray<FString>& MDATFileName) const 
{
	IFileManager::Get().FindFiles(MDATFileName, *FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("/MDATs/")));
	for (int i=0;i< MDATFileName.Num();i++)
		MDATFileName[i] = FPaths::GetCleanFilename(MDATFileName[i]);
}

bool UScoreEditorEntry::CreateNewMDAT(const FString& FileName)
{
	FString TargetFileName = ConstructMDATPath(FileName);
	if (IFileManager::Get().FileExists(*TargetFileName))
		return false;
	FMDATFile CurMDAT;
	return CurMDAT.Save(TargetFileName);
}

void UScoreEditorEntry::UpdateMDATInfo(const FString& FileName, FMDATMainInfo MDATInfo)
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

bool UScoreEditorEntry::TryGetMDATMainInfo(const FString& FileName, FMDATMainInfo& MDATInfo)
{
	FMDATFile tmpMDAT;
	if (!tmpMDAT.LoadFromFile(FileName))
		return false;
	return MuthMTypeHelper::TryGetMDATMainInfo(&tmpMDAT, MDATInfo);
}
