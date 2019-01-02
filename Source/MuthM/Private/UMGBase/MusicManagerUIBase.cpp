// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicManagerUIBase.h"
#include "MuthMBPLib.h"
#include "UIProvider.h"
#include "MusicImportationUIBase.h"
#include "MuthMGameInstance.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "MuthM"

TArray<FMusicInfo> UMusicManagerUIBase::GetLocalMusicInfo() const 
{
	return IMusicManager::Get(this)->GetLocalMusicList();
}

void UMusicManagerUIBase::DeleteMusic(int ID)
{
	IMusicManager::Get(this)->DeleteMusic(ID);
}

void UMusicManagerUIBase::ImportMusic()
{
	TArray<FString> Filters;
	Filters.Add("MP3");
	TArray<FString> SelectedFileName;
	if (!UMuthMBPLib::GetOpenFileName(LOCTEXT("Select Music", "Select Music"), Filters, false, SelectedFileName))
		return;
	//Currently only support import one music at once.
	auto UIClass = UUIProvider::Get(this)->GetMusicImportationUI();
	auto* UIInstance = Cast<UMusicImportationUIBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UIClass, "MusicImportationUI"));
	UIInstance->_MusicFileName = SelectedFileName[0];
	UIInstance->InitMusicInfo(SelectedFileName[0], "", "");
	UIInstance->OnMusicImportFinished.BindLambda([=]()
		{
			OnInitMusicList();
		});
	UIInstance->AddToViewport(102);
}

void UMusicManagerUIBase::UploadMusic(int ID, const FString& Title, const FString& Musician)
{
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	auto pSaveGame = GameInstance->GetGlobalSaveGame();
	auto* MusicInfo = pSaveGame->MusicInfoCollection.FindByPredicate([&](const FMusicInfo& a) {return a.ID == ID; });
	if (MusicInfo)
	{
		MusicInfo->Title = Title;
		MusicInfo->Musician = Musician;
	}
	else
		return;
	GameInstance->SaveGlobalSaveGame();
	IMusicManager::Get(this)->AddMusicUploadTask(ID, Title, Musician);
}

#undef LOCTEXT_NAMESPACE