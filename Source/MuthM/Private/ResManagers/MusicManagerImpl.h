// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ResManagers/MusicManager.h"
#include "MusicManagerImpl.generated.h"

/**
 * Implemention of IMusicManager
 */
UCLASS()
class UMusicManagerImpl : public UObject,public IMusicManager
{
	GENERATED_BODY()
		//
	FORCEINLINE FString ConstructMusicFileName(int MusicID) const
	{
		return FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), "/Music/", FString::FromInt(MusicID) + ".Opus");
	}
public:
	virtual bool LoadMusicDataByID(int MusicID, TArray<uint8>& MusicData) override;

	//Network Interface haven't been designed yet.
	virtual void FindMusicOnlineByID(int MusicID, FOnMusicQueryFinished QueryDelegate) override;
	virtual bool DownloadMusicByID(int MusicID) override;
	virtual bool IsMusicExistInLocal(int MusicID) const override;
	virtual bool FindMusicLocalByID(int MusicID, FMusicInfo& MusicInfo) const override;

};
