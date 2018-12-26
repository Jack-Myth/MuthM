// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MuthMBPLib.h"
#include "ScoreEditorEntry.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UScoreEditorEntry : public UUserWidget
{
	GENERATED_BODY()

	static FString ConstructMDATPath(const FString& MDATFileName);
public:
	UFUNCTION(BlueprintPure)
		void GetSuitableMDATs(TArray<FString>& MDATFileName) const;
	UFUNCTION(BlueprintCallable)
		bool CreateNewMDAT(const FString& FileName);
	UFUNCTION(BlueprintCallable,meta=(ToolTip="This function will not modify any score info,only MDAT info will be saved."))
		void UpdateMDATInfo(const FString& FileName,FMDATMainInfo MDATInfo);
	UFUNCTION(BlueprintPure)
		bool TryGetMDATMainInfo(const FString& FileName, FMDATMainInfo& MDATInfo);
};
