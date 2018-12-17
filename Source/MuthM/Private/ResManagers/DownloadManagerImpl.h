// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DownloadManager.h"
#include "DownloadManagerImpl.generated.h"

/**
 * 
 */
UCLASS()
class UDownloadManagerImpl : public UObject,public IDownloadManager
{
	GENERATED_BODY()

	UPROPERTY()
		TArray<class UDownloadTask*> DownloadList;
	FDownloadConfig DownloadConfig;
public:
	void LoadDownloadList();
	virtual class UDownloadTask* SubmitDownloadTask(const FString& DownloadURL, const FString& DestFileName,const FString& DisplayName=FString("")) override;
	virtual TArray<class UDownloadTask *> GetDownloadTasks() const override;
	virtual void PauseAllDownloadTasks() override;
	virtual void ResumeAllDownloadTasks() override;
	virtual FDownloadConfig GetDownloadConfig() const override;
	virtual void SetDownloadConfig(const FDownloadConfig& NewDownloadConfig) override;
	virtual void OnTaskFinishd() override;

};
