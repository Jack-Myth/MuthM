// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ContentManagerUIBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UContentManagerUIBase : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
		class UMusicManagerUIBase* MusicManagerUI;
	UPROPERTY()
		class UScoreManagerUIBase* ScoreManagerUI;
	UPROPERTY()
		class UModManagerUIBase* ModManagerUI;
public:
	UFUNCTION(BlueprintPure)
		class UMusicManagerUIBase* GetMusicManagerUI();
	UFUNCTION(BlueprintPure)
		class UScoreManagerUIBase* GetScoreManagerUI();
	UFUNCTION(BlueprintPure)
		class UModManagerUIBase* GetModManagerUI();
};
