// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DownloadManagerImpl.h"
#include "DownloadTask.h"

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
