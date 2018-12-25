// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "UploadTask.h"
#include "FileManager.h"
#include "DownloadManager.h"
#include "NetworkManager.h"
#include "IHttpRequest.h"
#include "FileHelper.h"
#include "MusicManager.h"
#include "MuthMBPLib.h"
#include "JsonObject.h"
#include "IHttpResponse.h"

void UUploadTask::UploadProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
	OnUploadProgress.Broadcast(BytesSent, FileLength);
}

void UUploadTask::UploadFinished(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	//No need to call INetworkManager::OnTaskFinished()
	UploadState = EUploadState::US_Finished;
	OnUploadStateChanged.Broadcast(UploadState);
	OnUploadFinished.Broadcast(this,bConnectedSuccessfully);
	
	//Action After Upload.
	switch (UploadType)
	{
		case EUploadType::DT_Music:
			{
				TSharedPtr<FJsonObject> JsonObj = UMuthMBPLib::DeserializeJsonFromStr(Response->GetContentAsString());
				if (JsonObj->GetIntegerField("ErrCode")!=0)
				{
					//Upload Failed
					IMusicManager::Get(this)->OnMusicUploaded(false, 0, ExternInfo);
					break;
				}
				int MusicID = JsonObj->GetIntegerField("MusicID");
				IMusicManager::Get(this)->OnMusicUploaded(true, MusicID, ExternInfo);
			}
			break;
		case EUploadType::DT_MDAT:
			break;
		case EUploadType::DT_Mod:
			break;
	}
}

UUploadTask* UUploadTask::CreateUploadTask(const UObject* WorldContextObj,const FString& LocalFileName, const FString& TargetURL, const FString& DataName, const FString& UploadName)
{
	if (!IFileManager::Get().FileExists(*LocalFileName))
		return nullptr;
	UUploadTask* UploadTask = NewObject<UUploadTask>(IDownloadManager::Get(WorldContextObj).GetObject());
	UploadTask->UploadName = UploadName;
	UploadTask->TargetURL = TargetURL;
	UploadTask->LocalFileName = LocalFileName;
	UploadTask->DataName = DataName;
	return UploadTask;
}

void UUploadTask::Start()
{
	if (UploadState != EUploadState::US_Stoped)
		return;
	TArray<uint8> boundary;
	const char* BoundaryChar = "----MuthMGameDownloadManager";
	const char* CCH = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	boundary.Append((const uint8*)BoundaryChar, 28); //strlen(BoundaryChar) will be 28
	for (int i=0;i<16;i++)
		boundary.Add(CCH[FMath::Rand() % (62)]); //[0~(10+26+26))
	UploadRequest = INetworkManager::Get(this)->GenRequest();
	UploadRequest->SetURL(TargetURL);
	UploadRequest->SetVerb("POST");
	UploadRequest->SetHeader("Content-Type", FString("multipart/form-data; boundary=")+FString((char*)boundary.GetData()));
	//Add file to Post
	//It should support multipart,but currently single part is enough.
	TArray<uint8> PostData;
	TArray<uint8> FileContent;
	FFileHelper::LoadFileToArray(FileContent, *LocalFileName);
	FileLength = FileContent.Num();
	PostData.Append((const uint8*)"--", 2);
	PostData.Append(boundary);
	PostData.Append((const uint8*)"\r\n", 2);
	PostData.Append((const uint8*)"Content-Disposition: form-data; name=\"", 38);
	PostData.Append((const uint8*)TCHAR_TO_ANSI(*DataName),DataName.Len());
	PostData.Append((const uint8*)"\"\r\n\r\n", 5);
	PostData.Append(FileContent);
	UploadRequest->SetContent(PostData);
	UploadRequest->OnRequestProgress().BindUObject(this, &UUploadTask::UploadProgress);
	UploadRequest->OnProcessRequestComplete().BindUObject(this, &UUploadTask::UploadFinished);
	UploadRequest->ProcessRequest();
	UploadState = EUploadState::US_Uploading;
	OnUploadStateChanged.Broadcast(UploadState);
}

void UUploadTask::Stop()
{
	if (UploadState != EUploadState::US_Uploading)
	{
		UploadRequest->CancelRequest();
		UploadRequest.Reset();
		UploadState = EUploadState::US_Stoped;
		OnUploadStateChanged.Broadcast(UploadState);
	}
}

void UUploadTask::Cancel()
{
	Stop();
	switch (UploadType)
	{
		case EUploadType::DT_Music:
			IMusicManager::Get(this)->OnMusicUploaded(false, -1, ExternInfo);
			break;
		case EUploadType::DT_MDAT:
			break;
		case EUploadType::DT_Mod:
			break;
	}
}
