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
	int ID;
	UPROPERTY(BlueprintReadWrite)
		FString Title;
	UPROPERTY(BlueprintReadWrite)
		FString Musician;
	UPROPERTY(BlueprintReadWrite)
		FString Description;
	int Size;
	FString DownloadURL;
	FName FormatType;
};

/**
 * Control Music's Search,Load etc.
 */
class MUTHM_API IMusicManager
{
	GENERATED_BODY()

public:
	static TScriptInterface<IMusicManager> Get();
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Get MusicData By ID,return Opus format Music"))
		virtual bool LoadMusicDataByID(int MusicID, TArray<uint8>& MusicData) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool FindMusicOnlineByID(int MusicID, FMusicInfo& MusicInfo) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool DownloadMusicByID(int MusicID) = 0;
};
