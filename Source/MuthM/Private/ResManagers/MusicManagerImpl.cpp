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

DEFINE_LOG_CATEGORY(MuthMMusicManager)

bool UMusicManagerImpl::LoadMusicDataByID(int MusicID, TArray<uint8>& MusicData)
{
	return FFileHelper::LoadFileToArray(MusicData,*ConstructMusicFileName(MusicID));
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
	//TODO:DownloadMusicByID
	return false;
}

bool UMusicManagerImpl::IsMusicExistInLocal(int MusicID) const
{
	if (IFileManager::Get().FileExists(*ConstructMusicFileName(MusicID)))
	{
		auto pSaveGame = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this))->GetGlobalSaveGame();
		for (int i=0;i<pSaveGame->MusicInfoCollection.Num();i++)
		{
			if (pSaveGame->MusicInfoCollection[i].ID == MusicID)
				return true;
		}
		//return false;
	}
	return false;
}

bool UMusicManagerImpl::FindMusicLocalByID(int MusicID, FMusicInfo& MusicInfo) const
{
	if (IFileManager::Get().FileExists(*ConstructMusicFileName(MusicID)))
	{
		auto pSaveGame = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this))->GetGlobalSaveGame();
		for (int i = 0; i < pSaveGame->MusicInfoCollection.Num(); i++)
		{
			if (pSaveGame->MusicInfoCollection[i].ID == MusicID)
			{
				MusicInfo = pSaveGame->MusicInfoCollection[i];
				return true;
			}
		}
		//return false;
	}
	return false;
}