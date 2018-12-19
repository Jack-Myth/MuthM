// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MusicManagerUIBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UMusicManagerUIBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		TArray<FMusicInfo> GetLocalMusicInfo();
	UFUNCTION(BlueprintCallable)
		bool DeleteLocalMusic(int MusicID);
	UFUNCTION(BlueprintCallable)
		bool ImportMusic();
};
