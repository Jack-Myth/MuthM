// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicManagerImpl.h"
#include "FileHelper.h"
#include "Paths.h"
#include "BulkData.h"
#include "Sound/SoundWave.h"
#include "OpusAudioInfo.h"
#include "TargetPlatform/Public/Interfaces/IAudioFormat.h"
#include "NetworkManager.h"
#include "IHttpRequest.h"
#include "IHttpResponse.h"
#include "MuthMBPLib.h"
#include "BlueprintJsonLibrary.h"
#include "JsonObject.h"
#include "FileManager.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMGameInstance.h"
#include "GlobalSaveGame.h"
#include "DownloadManager.h"

DEFINE_LOG_CATEGORY(MuthMMusicManager)

void UMusicManagerImpl::PrepareDownloadMusic(int MusicID, bool IsMusicExist,const FMusicInfo& MusicInfo)
{
	if (!IsMusicExist)
		return;
	auto* DownloadTask = IDownloadManager::Get(this)->SubmitDownloadTask(
		FString::Printf(TEXT("%s/get_music.php?MusicID=%d"), TEXT(MUTHM_URL_ROOT), MusicID), ConstructMusicFileName(MusicID),MusicInfo.Title);
	if (DownloadTask)
	{
		auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
		auto pSaveGame = GameInstance->GetGlobalSaveGame();
		pSaveGame->MusicInfoCollection.Add(MusicInfo);
		GameInstance->SaveGlobalSaveGame();
	}
}

bool UMusicManagerImpl::LoadMusicDataByID(int ID, TArray<uint8>& MusicData)
{
	if (ID>=0)
		return FFileHelper::LoadFileToArray(MusicData,*ConstructMusicFileName(ID));
	else
		return FFileHelper::LoadFileToArray(MusicData, *ConstructOfflineMusicFileName(ID));
}

void UMusicManagerImpl::FindMusicOnlineByID(int MusicID, FOnMusicQueryFinished QueryDelegate)
{
	//Maybe Use Lambda is more convenience
	auto Request = INetworkManager::Get(this)->GenRequest();
	Request->SetVerb("GET");
	Request->SetURL(FString(MUTHM_URL_ROOT) + "/query_music_byid.php?MusicID="+FString::FromInt(MusicID));
	Request->OnProcessRequestComplete().CreateLambda([MusicID, QueryDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (bConnectedSuccessfully)
			{
				FBlueprintJsonObject JsonObj = UMuthMBPLib::LoadJsonFromStr(Response->GetContentAsString());
				int ErrCode = JsonObj.Object->GetIntegerField("ErrCode");
				if (ErrCode == 0)
				{
					FMusicInfo tmpMusicInfo;
					tmpMusicInfo.ID = JsonObj.Object->GetIntegerField("ID");
					tmpMusicInfo.Title = JsonObj.Object->GetStringField("Title");
					tmpMusicInfo.Musician = JsonObj.Object->GetStringField("Musician");
					tmpMusicInfo.FormatType = "Opus";
					tmpMusicInfo.IsOffline = false;
					//TODO:Still have a size and description,Are they really needed?
					QueryDelegate.ExecuteIfBound(MusicID, true, tmpMusicInfo);
				}
				else
					QueryDelegate.ExecuteIfBound(MusicID, false, FMusicInfo());
			}
			else
				QueryDelegate.ExecuteIfBound(MusicID, false,FMusicInfo());
		});
}

bool UMusicManagerImpl::DownloadMusicByID(int MusicID)
{
	if (MusicID < 0)
		return false;
	FMusicInfo MusicInfo;
	if (FindMusicLocalByID(MusicID, MusicInfo))
		return true;
	//If not downloaded yet.
	FOnMusicQueryFinished OnMusicQueryDelegate;
	OnMusicQueryDelegate.BindUFunction(this, "PrepareDownloadMusic");
	FindMusicOnlineByID(MusicID, OnMusicQueryDelegate);
	return true;
}

bool UMusicManagerImpl::FindMusicLocalByID(int ID, FMusicInfo& MusicInfo) const
{
	if (ID >= 0)
	{
		//Music ID
		if (IFileManager::Get().FileExists(*ConstructMusicFileName(ID)))
		{
			auto pSaveGame = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this))->GetGlobalSaveGame();
			for (int i = 0; i < pSaveGame->MusicInfoCollection.Num(); i++)
			{
				if (pSaveGame->MusicInfoCollection[i].ID == ID)
				{
					MusicInfo = pSaveGame->MusicInfoCollection[i];
					return true;
				}
			}
			//return false;
		}
		return false;
	}
	else
	{
		//Offline ID
		if (IFileManager::Get().FileExists(*ConstructOfflineMusicFileName(ID)))
		{
			auto pSaveGame = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this))->GetGlobalSaveGame();
			for (int i = 0; i < pSaveGame->MusicInfoCollection.Num(); i++)
			{
				if (pSaveGame->MusicInfoCollection[i].ID == ID)
				{
					MusicInfo = pSaveGame->MusicInfoCollection[i];
					return true;
				}
			}
			//return false;
		}
		return false;
	}
}

TArray<FMusicInfo> UMusicManagerImpl::GetLocalMusicList() const
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	auto pSaveGame = GameInstance->GetGlobalSaveGame();
	return pSaveGame->MusicInfoCollection;
}
