// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ResManagers/MusicManager.h"
#include "MusicManagerImpl.generated.h"

/**
 * 
 */
UCLASS()
class UMusicManagerImpl : public UObject,public IMusicManager
{
	GENERATED_BODY()

public:
	virtual bool LoadMusicDataByID(int MusicID, TArray<uint8>& MusicData) override;

	//Network Interface haven't been designed yet.
	virtual bool FindMusicOnlineByID(int MusicID, FMusicInfo& MusicInfo) override;
	virtual bool DownloadMusicByID(int MusicID) override;


	virtual bool IsMusicExistInLocal(int MusicID) override;

};
