// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MusicManager.h"
#include "MusicSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class UMusicSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TArray<FMusicInfo> MusicInfoCollection;
};
