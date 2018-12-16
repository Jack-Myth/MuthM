// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DownloadManagerImpl.h"

class UDownloadTask* UDownloadManagerImpl::SubmitDownloadTask(const FString& DownloadURL, const FString& DestFileName)
{
	//TODO:
	return nullptr;
}

TArray<class UDownloadTask *> UDownloadManagerImpl::GetDownloadTasks() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void UDownloadManagerImpl::PauseAllDownloadTasks()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void UDownloadManagerImpl::ResumeAllDownloadTasks()
{
	throw std::logic_error("The method or operation is not implemented.");
}

FDownloadConfig UDownloadManagerImpl::GetDownloadConfig() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void UDownloadManagerImpl::SetDownloadConfig(const FDownloadConfig& NewDownloadConfig)
{
	throw std::logic_error("The method or operation is not implemented.");
}
