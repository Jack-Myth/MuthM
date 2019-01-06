// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ResManagers/MusicManager.h"
#include "MusicManagerImpl.generated.h"

/**
 * Implemention of IMusicManager
 */
UCLASS(Abstract)
class UMusicManagerImpl : public UObject,public IMusicManager
{
	GENERATED_BODY()
		//
	static FString ConstructMusicFileName(int MusicID)
	{
		return FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("/Music/"), FString::FromInt(MusicID) + ".Ogg");
	}
	static FString ConstructOfflineMusicFileName(int ID)
	{
		return FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("/Music/Offline/"), FString::FromInt(ID) + ".Ogg");
	}
	UFUNCTION()
		void PrepareDownloadMusic(int MusicID, bool IsMusicExist, const FMusicInfo& MusicInfo);
public:
	virtual bool LoadMusicDataByID(int ID, TArray<uint8>& MusicData) override;

	//Network Interface haven't been designed yet.
	virtual void FindMusicOnlineByID(int MusicID, FOnMusicQueryFinished QueryDelegate) override;
	virtual bool DownloadMusicByID(int MusicID) override;
	virtual bool FindMusicLocalByID(int ID, FMusicInfo& MusicInfo) const override;
	virtual TArray<FMusicInfo> GetLocalMusicList() const override;
	virtual bool ImportMP3(const FString& LocalFileName,const FString& Title,const FString& Musician) override;
	virtual void ImportMP3Async(const FString& LocalFileName, const FString& Title, const FString& Musician, FOnMusicImportFinished OnImportFinishedDelegate) override;
	virtual void DeleteMusic(int ID) override;
	virtual void OnMusicDownloaded(bool IsSuccessful, const FString& ExternInfo) override;
	virtual void OnMusicUploaded(bool IsSuccessful, int MusicID, const FString& ExternInfo) override;
	virtual bool AddMusicUploadTask(int ID, const FString& Title, const FString& Musician) override;
	virtual bool UploadMusicLinkOnly(int ID, const FString& MusicURL, const FString& Title, const FString& Musician) override;
	virtual TSharedPtr<class FMOD::Sound *> LoadMainSoundByID(int ID) override;

};
