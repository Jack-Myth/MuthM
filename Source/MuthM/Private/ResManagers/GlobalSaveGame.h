// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MusicManager.h"
#include "GlobalSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class UGlobalSaveGame : public USaveGame
{
	GENERATED_BODY()
		//Hold Global Info for Game, No need to split them up.

public:
	UPROPERTY()
		int UserID;
	UPROPERTY()
		FString Token;
	UPROPERTY()
		TArray<FString> LoginNameCollection;
	UPROPERTY()
		TArray<FMusicInfo> MusicInfoCollection;
	UPROPERTY()
		TArray<FString> LocalDownloadFileCollection;
};
