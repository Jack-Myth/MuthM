// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MusicManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMMusicManager,Log,All)

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UMusicManager : public UInterface
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FMusicInfo
{
	GENERATED_BODY()
public:
	//For Offline Music,ID will be a Temporary ID,It will change after upload to server.
	//Offline ID will always less than 0;
	UPROPERTY(BlueprintReadWrite)
		int ID;
	UPROPERTY(BlueprintReadWrite)
		FString Title;
	UPROPERTY(BlueprintReadWrite)
		FString Musician;
	UPROPERTY(BlueprintReadWrite)
		FString Description;
	UPROPERTY(BlueprintReadWrite,meta=(ToolTip="Mark Music is or not exist on remote server."))
		bool IsOffline=false;
	int Size;
	FName FormatType;
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnMusicQueryFinished, int, MusicID, bool, IsMusicExist,const FMusicInfo&, MusicInfo);
DECLARE_DELEGATE_TwoParams(FOnMusicImportFinished,bool, const FMusicInfo&);

/**
 * Control Music's Search,Load etc.
 */
class MUTHM_API IMusicManager
{
	GENERATED_BODY()

public:
	static TScriptInterface<IMusicManager> Get(const UObject* WorldContextObj);
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Get MusicData By ID,return Opus format Music"))
		virtual bool LoadMusicDataByID(int ID, TArray<uint8>& MusicData) = 0;
	UFUNCTION(BlueprintCallable)
		virtual void FindMusicOnlineByID(int MusicID, FOnMusicQueryFinished QueryDelegate) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool FindMusicLocalByID(int ID, FMusicInfo& MusicInfo) const = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool DownloadMusicByID(int MusicID) = 0;
	UFUNCTION(BlueprintCallable)
		virtual TArray<FMusicInfo> GetLocalMusicList() const =0;
	UFUNCTION()
		virtual bool ImportMP3(const FString& LocalFileName, const FString& Title, const FString& Musician) = 0;
	//Async Import MP3
	virtual void ImportMP3Async(const FString& LocalFileName, const FString& Title, const FString& Musician, FOnMusicImportFinished OnImportFinishedDelegate) = 0;
	UFUNCTION()
		virtual void DeleteMusic(int ID) = 0;
};
