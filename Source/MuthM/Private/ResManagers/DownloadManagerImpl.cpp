// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DownloadManagerImpl.h"
#include "DownloadTask.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameInstance.h"
#include "GlobalSaveGame.h"
#include "Paths.h"
#include "UploadTask.h"

void UDownloadManagerImpl::LoadDownloadList()
{
	//If DownloadList not empty,The download list must already been loaded.
	//It shouldn't be called second time.
	//TODO: It may can be removed.
	if (DownloadList.Num())
		return;
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	auto pSaveGame = GameInstance->GetGlobalSaveGame();
	for (int i = 0; i < pSaveGame->LocalDownloadFileCollection.Num(); i++)
	{
		UDownloadTask* DownloadTask = UDownloadTask::ParseDownloadTask(this, pSaveGame->LocalDownloadFileCollection[i]);
		if (DownloadTask) //It may failed to construct.
			DownloadList.Add(DownloadTask);
	}
}

class UDownloadTask* UDownloadManagerImpl::SubmitDownloadTask(const FString& DownloadURL, const FString& DestFileName, const FString& DisplayName/*=FString("")*/)
{
	UDownloadTask* DownloadTask;
	if (DisplayName == "")
		DownloadTask = UDownloadTask::CreateDownloadTask(this, DownloadURL, DestFileName, FPaths::GetCleanFilename(DestFileName));
	else
		DownloadTask = UDownloadTask::CreateDownloadTask(this, DownloadURL, DestFileName, DisplayName);
	if (DownloadTask)
	{
		DownloadList.Add(DownloadTask);
		//Add DownloadItem To SaveGame.
		auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
		auto pSaveGame = GameInstance->GetGlobalSaveGame();
		pSaveGame->LocalDownloadFileCollection.Add(DestFileName);
		GameInstance->SaveGlobalSaveGame();
	}
	return DownloadTask;
}

TArray<class UDownloadTask *> UDownloadManagerImpl::GetDownloadTasks() const
{
	return DownloadList;
}

void UDownloadManagerImpl::PauseAllDownloadTasks()
{
	for (int i=0;i<DownloadList.Num();i++)
		DownloadList[i]->Stop();
}

void UDownloadManagerImpl::ResumeAllDownloadTasks()
{
	for (int i = 0; i < DownloadList.Num(); i++)
		DownloadList[i]->Start();
}

FDownloadConfig UDownloadManagerImpl::GetDownloadConfig() const
{
	return DownloadConfig;
}

void UDownloadManagerImpl::SetDownloadConfig(const FDownloadConfig& NewDownloadConfig)
{
	DownloadConfig = NewDownloadConfig;
}

void UDownloadManagerImpl::OnTaskFinishd()
{
	//Remove Download Record from SaveGame,but didn't from the list.
	//The finished task should leave at list to let user see it
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	auto pSaveGame = GameInstance->GetGlobalSaveGame(); 
	for (int i = 0; i < DownloadList.Num(); i++)
	{
		if (DownloadList[i]->GetDownloadState() == EDownloadState::DS_Finished)
			pSaveGame->LocalDownloadFileCollection.Remove(DownloadList[i]->GetLocalFileName());
	}
	//XXX: Code is out of CriticalSection
	GameInstance->SaveGlobalSaveGame();
}

class UUploadTask* UDownloadManagerImpl::SubmitUploadTask(const FString& LocalFileName, const FString& UploadURL, const FString& DataName, const FString& DisplayName /*= FString("")*/)
{
	UUploadTask* UploadTask;
	if (DisplayName=="")
		UploadTask = UUploadTask::CreateUploadTask(this, LocalFileName, UploadURL, DataName, FPaths::GetCleanFilename(LocalFileName));
	else
		UploadTask = UUploadTask::CreateUploadTask(this, LocalFileName, UploadURL, DataName, DisplayName);
	if (UploadTask)
		UploadList.Add(UploadTask);
	return UploadTask;
}

void UDownloadManagerImpl::CancelDownloadTask(class UDownloadTask* DownloadTask)
{
	DownloadTask->CancelTask();
	DownloadList.Remove(DownloadTask);
}

void UDownloadManagerImpl::RemoveDownloadTask(class UDownloadTask* DownloadTask)
{
	if (DownloadTask->GetDownloadState() != EDownloadState::DS_Finished)
		return;
	DownloadList.Remove(DownloadTask);
}

void UDownloadManagerImpl::RemoveUploadTask(class UUploadTask* UploadTask)
{
}
