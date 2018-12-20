// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MusicImportationUIBase.generated.h"

DECLARE_DELEGATE(FOnMusicImportFinished);

/**
 * 
 */
UCLASS()
class MUTHM_API UMusicImportationUIBase : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnMusicImportFinished OnMusicImportFinished;
	FString MusicFileName;
	UFUNCTION(BlueprintCallable)
		void BeginImportMusic(const FString& Title, const FString& Musician);
};
