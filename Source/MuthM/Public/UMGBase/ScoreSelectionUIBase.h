// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreSelectionUIBase.generated.h"

USTRUCT(BlueprintType)
struct FScoreSelectionInfo
{
	GENERATED_BODY()

	TSharedPtr<class FMDATFile> pMDAT;
	UPROPERTY(BlueprintReadOnly)
		int ScoreNumber;
	UPROPERTY(BlueprintReadOnly)
		FText DisplayName;
	UPROPERTY(BlueprintReadOnly)
		FString ImgPath;
	UPROPERTY(BlueprintReadOnly)
		TArray<FName> RequestMods;
	UPROPERTY(BlueprintReadOnly)
		TArray<FName> OptionalMods;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMusicSelected, FScoreSelectionInfo, SelectedMusicInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMusicSelectionCancelled);

/**
 * 
 */
UCLASS()
class MUTHM_API UScoreSelectionUIBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		TArray<FScoreSelectionInfo> CollectScores();
};
