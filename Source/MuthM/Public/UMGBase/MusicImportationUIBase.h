// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MusicImportationUIBase.generated.h"

DECLARE_DELEGATE(FUIOnMusicImportFinished);

/**
 * 
 */
UCLASS()
class MUTHM_API UMusicImportationUIBase : public UUserWidget
{
	GENERATED_BODY()
public:
	FUIOnMusicImportFinished OnMusicImportFinished;
	FString MusicFileName;
	UFUNCTION(BlueprintCallable)
		void BeginImportMusic(const FString& Title, const FString& Musician);
	UFUNCTION(BlueprintImplementableEvent)
		void InitMusicInfo(const FString& MusicFileName,const FString& Title, const FString& Musician);
	UFUNCTION(BlueprintImplementableEvent)
		void OnAsyncImportStarted();
	UFUNCTION(BlueprintImplementableEvent)
		void OnAsyncImportEnd();
};
