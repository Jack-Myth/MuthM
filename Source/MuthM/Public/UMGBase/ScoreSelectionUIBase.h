// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MuthMBPLib.h"
#include "ScoreSelectionUIBase.generated.h"

DECLARE_DELEGATE_TwoParams(FOnScoreSelected, const FString&, int);
DECLARE_DELEGATE(FOnSelectionCancelled);

/**
 * It may contained Design Mistakes.
 * Should be more configurable,instead of using fixed ExpandableArea.
 */
UCLASS()
class MUTHM_API UScoreSelectionUIBase : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnScoreSelected OnScoreSelected;
	FOnSelectionCancelled OnSelectionCancelled;
	UFUNCTION(BlueprintImplementableEvent)
		void OnRestoreScoreSelection(const FString& MDATFilePath, int ScoreIndex);
	UFUNCTION(BlueprintImplementableEvent)
		void Init(const TMap<FString,FMDATMainInfo>& MDATMainInfo);
protected:
	bool bCheckModRequirement = true;
	TArray<FString> CollectMDAT();
	UFUNCTION(BlueprintCallable)
		void SelectScore(const FString& MDATName, int ScoreIndex);
	UFUNCTION(BlueprintCallable)
		void CancelSelection();

	virtual void NativeOnInitialized() override;

};
