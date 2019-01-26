// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ScoreSelectionUIBase.h"
#include "MuthMType.h"
#include "MuthMBPLib.h"
#include "ExpandableArea.h"
#include "UIProvider.h"
#include "GenericSelectionItemBase.h"
#include "VerticalBox.h"
#include "Paths.h"
#include "FileManager.h"
#include "MusicManager.h"
#include "JsonObject.h"
#include "MuthMTypeHelper.h"

TArray<FString> UScoreSelectionUIBase::CollectMDAT()
{
	TArray<FString> MDATFiles;
	IFileManager::Get().FindFiles(MDATFiles, *FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("/MDATs/")), TEXT(".MDAT"));
	return MDATFiles;
}

void UScoreSelectionUIBase::SelectScore(const FString& MDATName, int ScoreIndex)
{
	OnScoreSelected.ExecuteIfBound(MDATName, ScoreIndex);
}

void UScoreSelectionUIBase::CancelSelection()
{
	OnSelectionCancelled.ExecuteIfBound();
}

void UScoreSelectionUIBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	TMap<FString, FMDATMainInfo> MDATMainInfoMap;
	TArray<FString> MDATCollection;
	MDATCollection = CollectMDAT();
	for (int i = 0; i < MDATCollection.Num(); i++)
	{
		//Collect All MDAT Info.
		FMDATFile tmpMDAT;
		tmpMDAT.LoadFromFile(FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("/MDATs/"), MDATCollection[i]));
		FMDATMainInfo MDATMainInfo;
		//Use helper will be more convenient.
		MuthMTypeHelper::TryGetMDATMainInfo(&tmpMDAT, MDATMainInfo);
		MDATMainInfoMap.FindOrAdd(MDATCollection[i]) = MDATMainInfo;
	}
	Init(MDATMainInfoMap);
}
