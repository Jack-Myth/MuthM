// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DownloadTask.h"
#include "FileManager.h"
#include "Runnable.h"
#include "DownloadManager.h"
#include "NetworkManager.h"
#include "IHttpRequest.h"
#include "IHttpResponse.h"
#include "PlatformFilemanager.h"
#include "GenericPlatformFile.h"
#include "MuthMTypeHelper.h"
#include "FileHelper.h"

bool UDownloadTask::CheckIfDownloadRecordExist(const FString& DestFileName)
{
	return IFileManager::Get().FileExists(*(DestFileName + ".mmdownloadrecord"));
}

UDownloadTask* UDownloadTask::Internal_ParseDownloadTask(const UObject* WorldContextObj,const FString& DestFileName)
{
	TArray<uint8> DownloadRecordData;
	int RecordDataIndex=0;
	if (FFileHelper::LoadFileToArray(DownloadRecordData, *(DestFileName + ".mmdownloadrecord")))
	{
		UDownloadTask* CurDownloadTask = NewObject<UDownloadTask>(IDownloadManager::Get(WorldContextObj).GetObject());
		FUTF8ToTCHAR URLConvertion((const ANSICHAR*)DownloadRecordData.GetData());
		CurDownloadTask->DownloadRecord.DestFileName = DestFileName;
		CurDownloadTask->DownloadRecord.TargetURL = URLConvertion.Get();
		RecordDataIndex += URLConvertion.Length()+1; //Terminal Character.
		//Get FileSize.
		CurDownloadTask->DownloadRecord.FileSize = MuthMTypeHelper::LoadIntFromData(DownloadRecordData.GetData() + RecordDataIndex);
		RecordDataIndex += sizeof(int32);
		//Parse Download Record
		while (RecordDataIndex<DownloadRecordData.Num())
		{
			TPair<int32, int32> tmpBlock;
			int32 BlockLEdge = MuthMTypeHelper::LoadIntFromData(DownloadRecordData.GetData() + RecordDataIndex);
			RecordDataIndex += sizeof(int32);
			tmpBlock.Key = BlockLEdge;
			int32 BlockREdge = MuthMTypeHelper::LoadIntFromData(DownloadRecordData.GetData() + RecordDataIndex);
			tmpBlock.Value = BlockREdge;
			RecordDataIndex += sizeof(int32);
			CurDownloadTask->DownloadRecord.DownloadedBlocks.Add(tmpBlock);
		}
		return CurDownloadTask;
	}
	else
		return nullptr;
}

void UDownloadTask::OnConnected(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	CS_Mutex.Lock();
	if (Response->GetResponseCode() == 302||Response->GetResponseCode()==301)
	{
		FString RedirectURL = Response->GetHeader("Location");
		DownloadRecord.TargetURL = RedirectURL;
		Request->SetURL(RedirectURL);
		Request->ProcessRequest();
	}
	else if (Response->GetResponseCode() == 206)
	{
		int32 FileSize = Response->GetContentLength();
		DownloadRecord.FileSize = FileSize;
		DownloadState = EDownloadState::DS_CreatingFile;
		NotifyDownloadStateChanged();
		CreateFile();
		ReProgressThreads();
		bResumable = true;
		//XXX:Cross-Thread Warning.
		DownloadState = EDownloadState::DS_Connecting;
		NotifyDownloadStateChanged();
	}
	else if (Response->GetResponseCode() == 200)
	{
		int32 FileSize = Response->GetContentLength();
		DownloadRecord.FileSize = FileSize;
		bResumable = false;
		OnDownloadStateChanged.Broadcast(EDownloadState::DS_CreatingFile);
		CreateFile();
		FDownloadThreadData tmpD;
		int TargetIndex = DownloadRecord.DownloadedBlocks.Add(TPair<int32,int32>(0,0));
		tmpD.DownloadBlock = &DownloadRecord.DownloadedBlocks[TargetIndex];
		tmpD.BlockSize = DownloadRecord.FileSize;
		tmpD.SaveFileImmediately = IDownloadManager::Get(this)->GetDownloadConfig().SaveFileImmediately;
		TSharedPtr<IHttpRequest> RequestPtr = INetworkManager::Get(this)->GenRequest();
		DownloadingRequests.Add(RequestPtr, tmpD);
		RequestPtr->OnRequestProgress().BindUObject(this, &UDownloadTask::DownloadProgress);
		RequestPtr->OnProcessRequestComplete().BindUObject(this, &UDownloadTask::ThreadFinished);
		RequestPtr->ProcessRequest();
		DownloadState = EDownloadState::DS_Downloading;
		NotifyDownloadStateChanged();
	}
	CS_Mutex.Unlock();
}

void UDownloadTask::NotifyDownloadProgress()
{
	int DownloadedBytes=0;
	for (int i=0;i<DownloadRecord.DownloadedBlocks.Num();i++)
	{
		DownloadedBytes += DownloadRecord.DownloadedBlocks[i].Value - DownloadRecord.DownloadedBlocks[i].Key;
	}
	OnDownloadProgress.Broadcast(DownloadedBytes, DownloadRecord.FileSize);
}

void UDownloadTask::DownloadProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
	CS_Mutex.Lock();
	FDownloadThreadData* CurData = DownloadingRequests.Find(Request);
	if (CurData->SaveFileImmediately)
	{
		int32 ReceivedBlockSize = BytesReceived - CurData->DownloadBlock->Value;
		CurFile->Seek(CurData->DownloadBlock->Value);
		//Save Block only
		CurFile->Write(&(Request->GetResponse()->GetContent()[CurData->DownloadBlock->Value]), ReceivedBlockSize);
		//These two should be the same
		CurData->DownloadBlock->Value = FMath::Min(BytesReceived,CurData->BlockSize);
	}
	NotifyDownloadProgress();
	CS_Mutex.Unlock();
}

void UDownloadTask::DownloadFinish()
{
	//Flush and delete Info File.
	CurFile->Flush();
	CurFile.Reset();
	IFileManager::Get().Delete(*(DownloadRecord.DestFileName + ".mmdownloadrecord"));
	DownloadState = EDownloadState::DS_Finished;
	NotifyDownloadStateChanged();
	OnDownloadFinished.Broadcast(DownloadRecord.FileSize);
}

void UDownloadTask::ThreadFinished(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	CS_Mutex.Lock();
	FDownloadThreadData* CurData = DownloadingRequests.Find(Request);
	int BytesReceived = Response->GetContentLength();
	int32 ReceivedBlockSize = BytesReceived - CurData->DownloadBlock->Value;
	CurFile->Seek(CurData->DownloadBlock->Value);
	//Save Block only
	CurFile->Write(&(Request->GetResponse()->GetContent()[CurData->DownloadBlock->Value]), ReceivedBlockSize);
	CurData->DownloadBlock->Value = FMath::Min(BytesReceived, CurData->BlockSize);
	SaveDownloadState(ReceivedBlockSize);
	DownloadingRequests.Remove(Request);
	ReProgressThreads();
	CS_Mutex.Unlock();
}

void UDownloadTask::ReProgressThreads()
{
	TArray<TPair<int32, int32>> DownloadedBlocks = DownloadRecord.DownloadedBlocks;
	//Predict Download block.
	for (auto it=DownloadingRequests.CreateIterator();it;++it)
	{
		for (int i=0;i<DownloadedBlocks.Num();i++)
		{
			if (it->Value.DownloadBlock->Key==DownloadedBlocks[i].Key)
			{
				DownloadedBlocks[i].Value = it->Value.DownloadBlock->Key + it->Value.BlockSize;
				break;
			}
		}
	}
	//Merge Block
	DownloadedBlocks.Sort([](const TPair<int32, int32>& a, const TPair<int32, int32>& b)
		{
			return a.Key < b.Key;
		});
	TArray<TPair<int32, int32>> BlocksRemain;
	int32 LastRemainBlockEdge=0;
	if (DownloadedBlocks.Num())
	{
		BlocksRemain.Add(TPair<int32, int32>(0, DownloadedBlocks[0].Key));
		LastRemainBlockEdge = DownloadedBlocks[0].Value + 1;
	}
	for (int i=0;i<DownloadedBlocks.Num()-1;i++)
	{
		BlocksRemain.Add(TPair<int32, int32>(DownloadedBlocks[i].Value + 1, DownloadedBlocks[i + 1].Key - 1));
		LastRemainBlockEdge = DownloadedBlocks[i + 1].Value + 1;
	}
	BlocksRemain.Add(TPair<int32, int32>(LastRemainBlockEdge, DownloadRecord.FileSize - 1));
	//Clear invalid blocks
	for (auto it=BlocksRemain.CreateIterator();it;++it)
	{
		if (it->Value - it->Key <= 0)
			it.RemoveCurrent();
	}
	//Split blocks if block's count is less than MaxThreads;
	int MaxThreads = IDownloadManager::Get(this)->GetDownloadConfig().MaxThreads;
	int32 MinBlockSize = IDownloadManager::Get(this)->GetDownloadConfig().MinBlockSize;
	if (!BlocksRemain.Num())
	{
		//If All Bytes are downloaded.
		DownloadFinish();
		return;
	}
	while (BlocksRemain.Num()<MaxThreads-DownloadingRequests.Num())
	{
		TPair<int32, int32> Biggest=BlocksRemain[0];
		for (int i=1;i<BlocksRemain.Num();i++)
		{
			if (Biggest.Value - Biggest.Key < BlocksRemain[i].Value - BlocksRemain[i].Key)
				Biggest = BlocksRemain[i];
		}
		//Break if the biggest block is smaller than MinBlockSize.
		if (Biggest.Value - Biggest.Key < MinBlockSize)
			break;
		BlocksRemain.Remove(Biggest);
		int32 Mid = FMath::Lerp<int32>(Biggest.Value, Biggest.Key, 0.5f);
		TPair<int32, int32> Little = TPair<int32,int32>(Mid + 1, Biggest.Value);
		Biggest.Value = Mid;
		BlocksRemain.Add(Biggest);
		BlocksRemain.Add(Little);
	}
	for (int i = 0; i < FMath::Min(MaxThreads - DownloadingRequests.Num(), BlocksRemain.Num());i++)
	{
		FDownloadThreadData tmpD;
		auto TmpBlock = BlocksRemain[i];
		TmpBlock.Value = 0;
		int TargetIndex = DownloadRecord.DownloadedBlocks.Add(TmpBlock);
		tmpD.DownloadBlock=&DownloadRecord.DownloadedBlocks[TargetIndex];
		tmpD.BlockSize = BlocksRemain[i].Value - BlocksRemain[i].Key;
		tmpD.SaveFileImmediately = IDownloadManager::Get(this)->GetDownloadConfig().SaveFileImmediately;
		TSharedPtr<IHttpRequest> RequestPtr = INetworkManager::Get(this)->GenRequest();
		DownloadingRequests.Add(RequestPtr, tmpD);
		RequestPtr->SetHeader("Range", FString::Printf(TEXT("bytes=%u-%u"), BlocksRemain[i].Key, BlocksRemain[i].Value));
		RequestPtr->OnRequestProgress().BindUObject(this, &UDownloadTask::DownloadProgress);
		RequestPtr->OnProcessRequestComplete().BindUObject(this, &UDownloadTask::ThreadFinished);
		RequestPtr->ProcessRequest();
	}
}

void UDownloadTask::CreateFile()
{
	CurFile = MakeShareable<IFileHandle>(FPlatformFileManager::Get().GetPlatformFile().OpenWrite(*DownloadRecord.DestFileName,false,false));
	uint8* EmptyMemory = new uint8[1024 * 1024]; //1M Empty Memory;
	FMemory::Memzero(EmptyMemory, 1024 * 1024);
	for (int i=0;i<int(DownloadRecord.FileSize/(1024*1024));i++)
	{
		CurFile->Write(EmptyMemory, 1024 * 1024); //1MB Empty Data
	}
	//Remain Bytes
	CurFile->Write(EmptyMemory, DownloadRecord.FileSize % (1024 * 1024));
}

void UDownloadTask::SaveDownloadState(int Contribution)
{
	SaveStateThreshold += Contribution;
	TSharedPtr<IFileHandle> CurInfoFile = MakeShareable<IFileHandle>(FPlatformFileManager::Get().GetPlatformFile().OpenWrite(*(DownloadRecord.DestFileName + ".mmdownloadrecord")));
	//Save URL
	if (SaveStateThreshold>IDownloadManager::Get(this)->GetDownloadConfig().SaveStateThreshold)
	{
		CurInfoFile->Seek(0);
		FTCHARToUTF8 tmpConversion(*DownloadRecord.TargetURL);
		CurInfoFile->Write((const uint8*)tmpConversion.Get(), tmpConversion.Length() + 1); //Include Terminal Character
	}
	//Save FileSize
	TArray<uint8> IntBytes = MuthMTypeHelper::SaveIntToData((uint32)DownloadRecord.FileSize);
	CurInfoFile->Write(IntBytes.GetData(), sizeof(int32));
	for (int i=0;i<DownloadRecord.DownloadedBlocks.Num();i++)
	{
		TArray<uint8> IntBytes = MuthMTypeHelper::SaveIntToData((uint32)DownloadRecord.DownloadedBlocks[i].Key);
		CurInfoFile->Write(IntBytes.GetData(), sizeof(int32));
		IntBytes = MuthMTypeHelper::SaveIntToData((uint32)DownloadRecord.DownloadedBlocks[i].Value);
		CurInfoFile->Write(IntBytes.GetData(), sizeof(int32));
	}
	//Close file will auto do flush.
	//CurInfoFile->Flush();;
	//Close File
	CurInfoFile.Reset();
}

UDownloadTask* UDownloadTask::CreateDownloadTask(const UObject* WorldContextObj,const FString& TargetURL, const FString& DestFileName)
{
	if (CheckIfDownloadRecordExist(DestFileName))
		return Internal_ParseDownloadTask(WorldContextObj,DestFileName);
	//else
	UDownloadTask* CurDownloadTask = NewObject<UDownloadTask>(IDownloadManager::Get(WorldContextObj).GetObject());
	CurDownloadTask->DownloadRecord.DestFileName = DestFileName;
	CurDownloadTask->DownloadRecord.TargetURL = TargetURL;
	return CurDownloadTask;
}

UDownloadTask* UDownloadTask::ParseDownloadTask(const UObject* WorldContextObj,const FString& DestFileName)
{
	if (CheckIfDownloadRecordExist(DestFileName))
		return Internal_ParseDownloadTask(WorldContextObj,DestFileName);
	else
		return nullptr;
}

void UDownloadTask::Start()
{
	if (GetDownloadState() != EDownloadState::DS_Paused)
		return;
	TSharedPtr<IHttpRequest> Request = INetworkManager::Get(this)->GenRequest();
	if (!DownloadRecord.FileSize)
	{
		//Request File Size
		Request->SetVerb("HEAD");
		Request->SetURL(DownloadRecord.TargetURL);
		DownloadState = EDownloadState::DS_Connecting;
		NotifyDownloadStateChanged();
		Request->ProcessRequest();
	}
	else
	{
		//Reopen File
		CurFile = MakeShareable(FPlatformFileManager::Get().GetPlatformFile().OpenWrite(*DownloadRecord.DestFileName));
		ReProgressThreads();
		DownloadState = EDownloadState::DS_Downloading;
		NotifyDownloadStateChanged();
	}
}

void UDownloadTask::Stop()
{
	CS_Mutex.Lock();
	TArray<TSharedPtr<IHttpRequest>> ptrDownloadingRequests;
	DownloadingRequests.GetKeys(ptrDownloadingRequests);
	for (int i=0;i<ptrDownloadingRequests.Num();i++)
	{
		//XXX:Should pay more attention on save downloaded data.
		ptrDownloadingRequests[i]->CancelRequest();
		ptrDownloadingRequests[i]->OnProcessRequestComplete().ExecuteIfBound(ptrDownloadingRequests[i],ptrDownloadingRequests[i]->GetResponse(), true);
	}
	SaveDownloadState(IDownloadManager::Get(this)->GetDownloadConfig().SaveStateThreshold);
	DownloadingRequests.Empty();
	DownloadState = EDownloadState::DS_Paused;
	//Flush and Close File
	CurFile->Flush();
	CurFile.Reset();
	NotifyDownloadStateChanged();
	CS_Mutex.Unlock();
}
