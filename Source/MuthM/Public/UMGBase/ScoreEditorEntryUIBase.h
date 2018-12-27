// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MuthMBPLib.h"
#include "ScoreEditorEntryUIBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UScoreEditorEntryUIBase : public UUserWidget
{
	GENERATED_BODY()

	TSharedPtr<class FMDATFile> _MDATInstance;
	static FString ConstructMDATPath(const FString& MDATFileName);
	
protected:
	UFUNCTION(BlueprintPure)
		void GetSuitableMDATs(TArray<FString>& MDATFileName) const;
	UFUNCTION(BlueprintCallable)
		bool CreateNewMDAT(const FString& FileName);
	UFUNCTION(BlueprintCallable,meta=(ToolTip="This function will not modify any score info,only MDAT info will be saved."))
		void UpdateMDATInfo(const FString& FileName,FMDATMainInfo MDATInfo);
	UFUNCTION(BlueprintCallable)
		bool OpenMDAT(const FString& FileName, FMDATMainInfo& MDATInfo);
	UFUNCTION(BlueprintPure)
		bool GetFileFromOpenedMDAT(const FString& FileName,TArray<uint8>& FileData) const;
	UFUNCTION(BlueprintPure)
		TArray<FString> GetOpenedMDATFileList();
	UFUNCTION(BlueprintCallable)
		void SaveOpenedMDATMainInfo(const FMDATMainInfo& MDATInfo);
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Please save MDAT before rename.\nNotice:This Function Will close opened MDATFile if rename successful,please reopen after call it."))
		bool RenameOpenedMDATFile(const FString& NewFileName);
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Need refresh MDAT List after Delete."))
		void DeleteOpenedMDATFile();
};
