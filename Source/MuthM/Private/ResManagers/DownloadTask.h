// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Map.h"
#include "IHttpRequest.h"
#include "DownloadTask.generated.h"

UENUM(BlueprintType)
enum class EDownloadState :uint8
{
	DS_CreatingFile UMETA(DisplayName="CreatingFile"),
	DS_Paused UMETA(DisplayName = "Paused"),
	DS_Connecting UMETA(DisplayName = "Connecting"),
	DS_Downloading UMETA(DisplayName = "Downloading"),
	DS_Finished UMETA(DisplayName="Finished")
};

UENUM()
enum class EDownloadType :uint8
{
	DT_None,
	DT_Music,
	DT_MDAT,
	DT_Mod
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDownloadProgress, int, DownloadedSize, int, TotalSize);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDownloadFinished,class UDownloadTask*,DownloadTask,int,bSuccessfully);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDownloadStateChanged, EDownloadState, DownloadState);

struct FDownloadRecord
{
	FString TargetURL;
	FString DestFileName;
	int32 FileSize;
	TArray<TPair<int32, int32>> DownloadedBlocks;
};

struct FDownloadThreadData
{
	TPair<int32, int32>* DownloadBlock;
	int32 BlockSize;
	bool SaveFileImmediately = true;
};

/**
 * Hold Download Task,Each Download Task means a download item.
 * Multi-Threads Download and Resume From Break-point are supported.
 */
UCLASS()
class UDownloadTask : public UObject
{
	GENERATED_BODY()
		//TODO: Currently it can't detect the modification for remote file
		//If the remote file changed,download may have unexpected action.
		//Need fix it in future,use ETag or something to detect modification.
	FDownloadRecord DownloadRecord;
	static bool CheckIfDownloadRecordExist(const FString& DestFileName);
	static UDownloadTask* Internal_ParseDownloadTask(const UObject* WorldContextObj,const FString& DestFileName);
	TMap<TSharedPtr<class IHttpRequest>,FDownloadThreadData> DownloadingRequests;
	TSharedPtr<class IFileHandle> CurFile;
	bool bResumable = false;
	int SaveStateThreshold=0;
	void OnConnected(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void NotifyDownloadProgress();
	void DownloadProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived);
	void DownloadFinish(bool bSuccessfully);
	//Write data to disk and update the record.
	//Analyze the DownloadedBlocks, create new thread if needed.
	void ThreadFinished(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void ReProgressThreads();
	void CreateFile();
	void SaveDownloadState(int Contribution);
	FORCEINLINE void NotifyDownloadStateChanged()
	{
		OnDownloadStateChanged.Broadcast(GetDownloadState());
	}
	EDownloadState DownloadState;
	FString DownloadName;
public:
	EDownloadType DownloadType = EDownloadType::DT_None;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> Tags;
	FString ExternInfomation;
	UPROPERTY(BlueprintAssignable)
		FOnDownloadProgress OnDownloadProgress;
	UPROPERTY(BlueprintAssignable)
		FOnDownloadFinished OnDownloadFinished;
	UPROPERTY(BlueprintAssignable)
		FOnDownloadStateChanged OnDownloadStateChanged;
	static UDownloadTask* CreateDownloadTask(const UObject* WorldContextObj,const FString& TargetURL, const FString& DestFileName, const FString& DisplayName);
	static UDownloadTask* ParseDownloadTask(const UObject* WorldContextObj,const FString& DestFileName);
	UFUNCTION(BlueprintCallable)
		void Start();
	UFUNCTION(BlueprintCallable)
		void Stop();
	UFUNCTION(BlueprintCallable)
		void CancelTask();
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsResumable() const
	{
		return bResumable;
	}
	UFUNCTION(BlueprintPure)
		FORCEINLINE EDownloadState GetDownloadState()
	{
		return DownloadState;
	}
	UFUNCTION(BlueprintPure)
		FORCEINLINE FString GetDownloadName()
	{
		return DownloadName;
	}
	UFUNCTION(BlueprintPure)
		FORCEINLINE FString GetLocalFileName()
	{
		return DownloadRecord.DestFileName;
	}
};
