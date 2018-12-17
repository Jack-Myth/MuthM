// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DownloadManagerImpl.h"
#include "DownloadTask.h"

void UDownloadManagerImpl::LoadDownloadList()
{
	//If DownloadList not empty,The download list must already been loaded.
	//It shouldn't be called second time.
	//TODO: It may can be removed.
	if (DownloadList.Num())
		return;
	
}

class UDownloadTask* UDownloadManagerImpl::SubmitDownloadTask(const FString& DownloadURL, const FString& DestFileName)
{
	UDownloadTask* DownloadTask = UDownloadTask::CreateDownloadTask(this, DownloadURL, DestFileName);
	DownloadList.Add(DownloadTask);
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
