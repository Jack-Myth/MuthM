// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ResManagers/DownloadTask.h"
#include "ResManagers/UploadTask.h"
#include "DownloadManager.generated.h"

USTRUCT(BlueprintType)
struct FDownloadConfig
{
	GENERATED_BODY()
		//
	UPROPERTY(BlueprintReadWrite)
		int32 MaxThreads = 4; //4 Threads Download.
	UPROPERTY(BlueprintReadWrite)
		int32 MinBlockSize = 1024 * 1024; //1MB
	UPROPERTY(BlueprintReadWrite)
		bool SaveFileImmediately = true;
	UPROPERTY(BlueprintReadWrite)
		int32 SaveStateThreshold = 512 * 1024; //512KB
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI,meta=(CannotImplementInterfaceInBlueprint))
class UDownloadManager : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MUTHM_API IDownloadManager
{
	GENERATED_BODY()
		
	//
public:
	static TScriptInterface<IDownloadManager> Get(const UObject* WorldContextObj);
	UFUNCTION(BlueprintCallable)
		virtual class UDownloadTask* SubmitDownloadTask(const FString& DownloadURL, const FString& DestFileName, const FString& DisplayName = FString(""), EDownloadType DownloadType = EDownloadType::DT_None, const FString& ExternInfo = "") = 0;
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Use HTML/FROM-DATA to upload task\nNotice:Upload Task is disposable, the upload record won't save and all unfinshed upload task will lost after exit game."))
		virtual class UUploadTask* SubmitUploadTask(const FString& LocalFileName, const FString& UploadURL, const FString& DataName, const FString& DisplayName = FString(""), EUploadType UploadType = EUploadType::DT_None, const FString& ExternInfo = "") = 0;
	UFUNCTION(BlueprintCallable)
		virtual TArray<class UDownloadTask*> GetDownloadTasks() const = 0;
	UFUNCTION(BlueprintCallable)
		virtual void PauseAllDownloadTasks() = 0;
	UFUNCTION(BlueprintCallable)
		virtual void ResumeAllDownloadTasks() = 0;
	UFUNCTION(BlueprintCallable)
		virtual FDownloadConfig GetDownloadConfig() const = 0;
	UFUNCTION(BlueprintCallable,meta=(ToolTip="It doesn't work until the Download task paused and resume."))
		virtual void SetDownloadConfig(const FDownloadConfig& NewDownloadConfig) = 0;
	UFUNCTION()
		virtual void OnTaskFinishd() = 0;
	UFUNCTION(BlueprintCallable)
		virtual void CancelDownloadTask(class UDownloadTask* DownloadTask) = 0;
	UFUNCTION(BlueprintCallable)
		virtual void CancelUploadTask(class UUploadTask* UploadTask) = 0;
	UFUNCTION(BlueprintCallable, meta=(ToolTip = "Notice: Only allow to remove finished task."))
		virtual void RemoveDownloadTask(class UDownloadTask* DownloadTask) = 0;
	UFUNCTION(BlueprintCallable, meta=(ToolTip = "Notice: Only allow to remove finished task."))
		virtual void RemoveUploadTask(class UUploadTask* UploadTask) = 0;
	UFUNCTION(BlueprintCallable)
		virtual TArray<class UDownloadTask*> FindDownloadTasksByTag(const FString& Tag) = 0;
	UFUNCTION(BlueprintCallable)
		virtual TArray<class UUploadTask*> FindUploadTasksByTag(const FString& Tag) = 0;
};
