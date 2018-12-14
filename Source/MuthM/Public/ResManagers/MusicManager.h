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
	FName FormatType;
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnMusicQueryFinished, int, MusicID, bool, IsMusicExist,const FMusicInfo&, MusicInfo);

/**
 * Control Music's Search,Load etc.
 */
class MUTHM_API IMusicManager
{
	GENERATED_BODY()

public:
	static TScriptInterface<IMusicManager> Get(const UObject* WorldContextObj);
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Get MusicData By ID,return Opus format Music"))
		virtual bool LoadMusicDataByID(int MusicID, TArray<uint8>& MusicData) = 0;
	UFUNCTION(BlueprintCallable)
		virtual void FindMusicOnlineByID(int MusicID, FOnMusicQueryFinished QueryDelegate) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool FindMusicLocalByID(int MusicID, FMusicInfo& MusicInfo) const = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool DownloadMusicByID(int MusicID) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool IsMusicExistInLocal(int MusicID) const = 0;
};
