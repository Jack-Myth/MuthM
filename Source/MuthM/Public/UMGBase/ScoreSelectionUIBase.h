// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreSelectionUIBase.generated.h"

USTRUCT()
struct FMDATSelectionInfo
{
	GENERATED_BODY()

	FString MDATPath;
	class UExpandableArea* HostWidget;
	FText DisplayName;
	FText Author;
	class UTexture2D* Cover=nullptr;
};

USTRUCT(BlueprintType)
struct FScoreSelectionInfo
{
	GENERATED_BODY()

	FString MDATPath;
	FString MMSPath;
	int MusicID;
	int ScoreNumber;
	FText DisplayName;
	FText Subtitle;
	class UTexture2D* Img;
	TArray<FName> RequestMods;
	TArray<FName> OptionalMods;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreSelected, FScoreSelectionInfo, SelectedScoreInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectionCancelled);

/**
 * 
 */
UCLASS()
class MUTHM_API UScoreSelectionUIBase : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
		TArray<FMDATSelectionInfo>  MDATSelectionCollection;
	UPROPERTY()
		TArray<FScoreSelectionInfo> ScoreSelectionCollection;
	int SelectedMDATIndex = INDEX_NONE;
	int SelectedScoreArrayIndex = INDEX_NONE;
public:
	UPROPERTY(BlueprintAssignable)
		FOnScoreSelected OnScoreSelected;
	UPROPERTY(BlueprintAssignable)
		FOnSelectionCancelled OnSelectionCancelled;
	TArray<FString> CollectMDAT();
	UFUNCTION(BlueprintImplementableEvent)
		void OnAddMDATItem(class UExpandableArea* ItemCategoryWidget);
	UFUNCTION()
		void OnMDATSelectedHandler(class UGenericSelectionItemBase* SelectedWidget, int DataIndex);
	UFUNCTION()
		void OnScoreSelectedHandler(class UGenericSelectionItemBase* SelectedWidget, int DataIndex);
protected:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintImplementableEvent)
		void DisplayMDATInfo(const FText& DisplayName,const FText& Author,const FText& Description, class UTexture2D* Cover);
	UFUNCTION(BlueprintImplementableEvent)
		void DisplayScoreInfo(const FText& DisplayName, const FText& Subtitle, int MusicID,class UTexture2D* Img, const TArray<FName>& RequestMods, const TArray<FName>& OptionalMods);
	UFUNCTION(BlueprintCallable)
		void SelectCurrentScore();
	UFUNCTION(BlueprintCallable)
		void CancelSelection();
};
