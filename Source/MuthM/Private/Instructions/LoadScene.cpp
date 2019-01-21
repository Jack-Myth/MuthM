// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "LoadScene.h"
#include "JsonObject.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "MuthM"

void ULoadScene::OnStringDetailsCallback(class UInstruction* InstructionInstance, FName PropertyName, FString StrValue)
{
	if (PropertyName=="OnLoadSceneName")
	{
		OnLoadSceneName = StrValue;
	}
	else if (PropertyName == "OnArrivedSceneName")
	{
		OnArrivedSceneName = StrValue;
	}
}

void ULoadScene::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	OnLoadSceneName = Args.Object->GetStringField("OnLoadSceneName");
	if (OnLoadSceneName !="")
	{
		bool bSuccessful;
		ULevelStreamingDynamic* LSD = ULevelStreamingDynamic::LoadLevelInstance(this, OnLoadSceneName, FVector::ZeroVector, FRotator::ZeroRotator, bSuccessful);
		if (bSuccessful&&LSD) //Maybe only need one?
		{
			LSD->SetShouldBeLoaded(true);
			LSD->SetShouldBeVisible(true);
			GetWorld()->AddStreamingLevel(LSD);
			//GetWorld()->UpdateLevelStreaming();
		}
	}
	OnArrivedSceneName = Args.Object->GetStringField("OnArrivedSceneName");
	if (OnArrivedSceneName!="")
	{
		bool bSuccessful;
		//Cached it to prevent freeze the game.
		CachedOnArrivedLevel = ULevelStreamingDynamic::LoadLevelInstance(this, OnArrivedSceneName, FVector::ZeroVector, FRotator::ZeroRotator, bSuccessful);
		if (bSuccessful)
		{
			CachedOnArrivedLevel->SetShouldBeLoaded(true);
			CachedOnArrivedLevel->SetShouldBeVisible(false);
			GetWorld()->AddStreamingLevel(CachedOnArrivedLevel);
		}
	}
	else
	{
		//If no scene need to be load later,Destroy self.
		DestroySelf();
	}
}

void ULoadScene::OnTimeArrived_Implementation()
{
	if (OnArrivedSceneName!=""&&CachedOnArrivedLevel)
	{
		CachedOnArrivedLevel->SetShouldBeVisible(true);
		DestroySelf();
	}
}

void ULoadScene::OnBuildingDetails_Implementation(TScriptInterface<IDetailsBuilder>& DetailsBuilder)
{
	Super::OnBuildingDetails_Implementation(DetailsBuilder);
	FDetailCategoryStruct DetailCategoryInto;
	DetailCategoryInto.DisplayTitle = LOCTEXT("LoadScene", "LoadScene");
	DetailCategoryInto.Title = "LoadScene";
	//OnLoadSceneName
	TSharedPtr<FDetailItemString> OnLoadSceneNameInfo = MakeShareable(new FDetailItemString());
	OnLoadSceneNameInfo->DetailCallbackStr.BindUFunction(this, "OnStringDetailsCallback");
	OnLoadSceneNameInfo->InstructionInstance = this;
	OnLoadSceneNameInfo->Name = "OnLoadSceneName";
	OnLoadSceneNameInfo->DisplayName = LOCTEXT("OnLoadSceneName", "OnLoadSceneName");
	DetailCategoryInto.ItemList.Add(OnLoadSceneNameInfo);

	TSharedPtr<FDetailItemString> OnArrivedSceneNameInfo = MakeShareable(new FDetailItemString());
	OnArrivedSceneNameInfo->DetailCallbackStr.BindUFunction(this, "OnStringDetailsCallback");
	OnArrivedSceneNameInfo->InstructionInstance = this;
	OnArrivedSceneNameInfo->Name = "OnArrivedSceneName";
	OnArrivedSceneNameInfo->DisplayName = LOCTEXT("OnArrivedSceneName", "OnArrivedSceneName");
	DetailCategoryInto.ItemList.Add(OnArrivedSceneNameInfo);
	DetailsBuilder->AddCategory(DetailCategoryInto);
}

#undef LOCTEXT_NAMESPACE