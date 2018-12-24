// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MusicManager.h"
#include "MusicManagerUIBase.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnMusicImported);

/**
 * 
 */
UCLASS()
class MUTHM_API UMusicManagerUIBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
		TArray<FMusicInfo> GetLocalMusicInfo() const;
	UFUNCTION(BlueprintCallable)
		void DeleteMusic(int ID);
	UFUNCTION(BlueprintCallable)
		void ImportMusic();
	UFUNCTION(BlueprintImplementableEvent,meta=(ToolTip="Notice:It may call many times to refresh the list after import music."))
		void OnInitMusicList();
};
