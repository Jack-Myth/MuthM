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

TArray<FString> UScoreSelectionUIBase::CollectMDAT()
{
	TArray<FString> MDATFiles;
	IFileManager::Get().FindFiles(MDATFiles, *FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), "/MDAT/"), TEXT(".MDAT"));
	return MDATFiles;
}

void UScoreSelectionUIBase::OnMDATSelectedHandler(class UGenericSelectionItemBase* SelectedWidget, int DataIndex)
{
	if (SelectedMDATIndex!=INDEX_NONE)
	{
		MDATSelectionCollection[SelectedMDATIndex].HostWidget->SetIsExpanded_Animated(false);
		MDATSelectionCollection[SelectedMDATIndex].HostWidget->GetContentForSlot("Body")->RemoveFromParent();
		SelectedMDATIndex = INDEX_NONE;
	}
	if (DataIndex >= 0 && DataIndex < MDATSelectionCollection.Num())
		SelectedMDATIndex = DataIndex;
	else
		return;
	ScoreSelectionCollection.Empty();
	//Add Scores To Body.
	UVerticalBox* tmpVerticleBox = NewObject<UVerticalBox>();
	FMDATFile tmpMDATFile;
	//MDATPath was checked at NativeConstruct,No need to check again.
	tmpMDATFile.LoadFromFile(MDATSelectionCollection[SelectedMDATIndex].MDATPath);
	TSharedPtr<FJsonObject> MainJsonObj = UMuthMBPLib::DeserializeJsonFromUTF8(tmpMDATFile.GetFileData("/Main"));
	int32 ScoreCount = MainJsonObj->GetIntegerField("ScoreCount");
	for (int i=0;i<ScoreCount;i++)
	{
		FScoreSelectionInfo CurScoreSelectionInfo;
		TSharedPtr<FJsonObject> ScoreInfo = MainJsonObj->GetObjectField(FString::Printf(TEXT("Score%d"), i));
		CurScoreSelectionInfo.ScoreIndex = i;
		CurScoreSelectionInfo.DisplayName = FText::FromString(ScoreInfo->GetStringField("ScoreName"));
		CurScoreSelectionInfo.Subtitle = FText::FromString(ScoreInfo->GetStringField("Subtitle"));
		const TArray<TSharedPtr<FJsonValue>> RequestModsArray = ScoreInfo->GetArrayField("RequestMods");
		for (int mi=0;mi<RequestModsArray.Num();mi++)
			CurScoreSelectionInfo.RequestMods.Add(*RequestModsArray[i]->AsString());
		const TArray<TSharedPtr<FJsonValue>> OptionalModsArray = ScoreInfo->GetArrayField("OptionalMods");
		for (int mi = 0; mi < OptionalModsArray.Num(); mi++)
			CurScoreSelectionInfo.OptionalMods.Add(*OptionalModsArray[i]->AsString());
		FString ImgPath;
		if (ScoreInfo->TryGetStringField("Img", ImgPath))
			CurScoreSelectionInfo.Img = UMuthMBPLib::GetLocalTextureByImageData(tmpMDATFile.GetFileData(ImgPath));
		else
			CurScoreSelectionInfo.Img = MDATSelectionCollection[SelectedMDATIndex].Cover;
		CurScoreSelectionInfo.MusicID = ScoreInfo->GetIntegerField("MusicID");
		UGenericSelectionItemBase* GSI = Cast<UGenericSelectionItemBase>(
			UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get(this)->GetGenericSelectionItem(), NAME_None));
		GSI->SetDataIndex(ScoreSelectionCollection.Add(CurScoreSelectionInfo));
		GSI->OnSelected.AddDynamic(this, &UScoreSelectionUIBase::OnScoreSelectedHandler);
		GSI->ApplyAppearance(CurScoreSelectionInfo.DisplayName, CurScoreSelectionInfo.Subtitle, CurScoreSelectionInfo.Img);
		FMusicInfo tmpMusicInfo;
		if (!IMusicManager::Get(this)->FindMusicLocalByID(CurScoreSelectionInfo.MusicID, tmpMusicInfo))
			GSI->ChangeIconBrightness(0.5f);
		tmpVerticleBox->AddChildToVerticalBox(GSI);
	}
	SelectedScoreArrayIndex = INDEX_NONE;
	MDATSelectionCollection[SelectedMDATIndex].HostWidget->SetContentForSlot("Body", tmpVerticleBox);
	MDATSelectionCollection[SelectedMDATIndex].HostWidget->SetIsExpanded_Animated(true);
	DisplayMDATInfo(MDATSelectionCollection[SelectedMDATIndex].DisplayName,
		MDATSelectionCollection[SelectedMDATIndex].Author, FText::FromString(MainJsonObj->GetStringField("Description"))
		, MDATSelectionCollection[SelectedMDATIndex].Cover);
	
}

void UScoreSelectionUIBase::OnScoreSelectedHandler(class UGenericSelectionItemBase* SelectedWidget, int DataIndex)
{
	SelectedScoreArrayIndex = DataIndex;
	DisplayScoreInfo(ScoreSelectionCollection[DataIndex].DisplayName,
		ScoreSelectionCollection[DataIndex].Subtitle,
		ScoreSelectionCollection[DataIndex].MusicID,
		ScoreSelectionCollection[DataIndex].Img,
		ScoreSelectionCollection[DataIndex].RequestMods,
		ScoreSelectionCollection[DataIndex].OptionalMods);
}

void UScoreSelectionUIBase::NativeConstruct()
{
	Super::NativeConstruct();
	MDATSelectionCollection.Empty(); //No use, because it always empty at first.
	ScoreSelectionCollection.Empty();
	TArray<FString> MDATCollection;
	MDATCollection = CollectMDAT();
	for (int i=0;i<MDATCollection.Num();i++)
	{
		FMDATFile tmpMDAT;
		tmpMDAT.LoadFromFile(MDATCollection[i]);
		if (tmpMDAT.IsFileExist("/Main"))
		{
			FMDATSelectionInfo MDATSelectionInfo;
			TSharedPtr<FJsonObject> MainJsonObj = UMuthMBPLib::DeserializeJsonFromUTF8(tmpMDAT.GetFileData("/Main"));
			//TODO: String To Text may need to change.
			MDATSelectionInfo.DisplayName = FText::FromString(MainJsonObj->GetStringField("Title"));
			MDATSelectionInfo.Author = FText::FromString(MainJsonObj->GetStringField("AuthorName"));
			FString CoverPath = MainJsonObj->GetStringField("Cover");
			if (tmpMDAT.IsFileExist(CoverPath))
				MDATSelectionInfo.Cover = UMuthMBPLib::GetLocalTextureByImageData(tmpMDAT.GetFileData(CoverPath));
			MDATSelectionCollection.Add(MDATSelectionInfo);
		}
	}
	for (int i=0;i<MDATSelectionCollection.Num();i++)
	{
		class UExpandableArea* MDATSelectionHost = NewObject<UExpandableArea>();
		MDATSelectionHost->BorderBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
		MDATSelectionHost->AreaPadding = 10;
		MDATSelectionHost->Style.SetRolloutAnimationSeconds(1.f);
		auto* SelectionItem = Cast<UGenericSelectionItemBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get(this)->GetGenericSelectionItem(), NAME_None));
		SelectionItem->SetDataIndex(i);
		SelectionItem->OnSelected.AddDynamic(this, &UScoreSelectionUIBase::OnMDATSelectedHandler);
		SelectionItem->ApplyAppearance(MDATSelectionCollection[i].DisplayName, MDATSelectionCollection[i].Author, MDATSelectionCollection[i].Cover);
		MDATSelectionHost->SetContentForSlot("Header",SelectionItem);
		MDATSelectionCollection[i].HostWidget = MDATSelectionHost;
		OnAddMDATItem(MDATSelectionHost);
	}
}

void UScoreSelectionUIBase::SelectCurrentScore()
{
	if (SelectedScoreArrayIndex != INDEX_NONE)
	{
		FMusicInfo tmpMusicInfo;
		if (IMusicManager::Get(this)->FindMusicLocalByID(ScoreSelectionCollection[SelectedScoreArrayIndex].MusicID,tmpMusicInfo))
		{
			OnScoreSelected.Broadcast(ScoreSelectionCollection[SelectedScoreArrayIndex]);
			OnNeedRemove();
		}
		else
		{
			OnMusicNotDownloadWarning.Broadcast(ScoreSelectionCollection[SelectedScoreArrayIndex].MusicID);
		}
	}
}

void UScoreSelectionUIBase::CancelSelection()
{
	OnSelectionCancelled.Broadcast();
	OnNeedRemove();
}
