// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IHttpRequest.h"
#include "UploadTask.generated.h"

UENUM(BlueprintType)
enum class EUploadState :uint8
{
	US_Stoped UMETA(DisplayName = "Stoped"),
	US_Uploading UMETA(DisplayName = "Uploading"),
	US_Finished UMETA(DisplayName = "Finished")
};

UENUM()
enum class EUploadType :uint8
{
	DT_None,
	DT_Music,
	DT_MDAT,
	DT_Mod
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUploadProgress, int, SentSize, int, TotalSize);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUploadFinished,class UUploadTask*,UploadTask, bool, bSuccessfully);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUploadStateChanged, EUploadState, UploadState);

/**
 * Hold Upload Task,Each Instance means one Upload Task
 * Doesn't allow pause and resume,but can stop and restart
 * Use HTTP/FROM-DATA to upload file.
 * Currently only support upload one file.
 */
UCLASS()
class UUploadTask : public UObject
{
	GENERATED_BODY()
		//
	UPROPERTY()
		FString UploadName;
	int FileLength;
	FString TargetURL;
	FString LocalFileName;
	EUploadState UploadState=EUploadState::US_Stoped;
	FString DataName;
	TSharedPtr<class IHttpRequest> UploadRequest;
	void UploadProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived);
	void UploadFinished(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
public:
	EUploadType UploadType;
	FString ExternInfo;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> Tags;
	UPROPERTY(BlueprintAssignable)
		FOnUploadProgress OnUploadProgress;
	UPROPERTY(BlueprintAssignable)
		FOnUploadFinished OnUploadFinished;
	UPROPERTY(BlueprintAssignable)
		FOnUploadStateChanged OnUploadStateChanged;
	static UUploadTask* CreateUploadTask(const UObject* WorldContextObj,const FString& LocalFileName, const FString& TargetURL,const FString& DataName, const FString& UploadName);
	UFUNCTION(BlueprintCallable)
		void Start();
	UFUNCTION(BlueprintCallable)
		void Stop();
	UFUNCTION(BlueprintPure)
	FORCEINLINE EUploadState GetUploadState() const
	{
		return UploadState;
	}
	UFUNCTION()
		void Cancel();
};
