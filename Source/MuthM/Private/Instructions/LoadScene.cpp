// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "LoadScene.h"
#include "JsonObject.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/World.h"
#include "PackageName.h"
#include "MuthMBPLib.h"

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
	OnArrivedSceneName = Args.Object->GetStringField("OnArrivedSceneName");
	if (OnLoadSceneName !="")
	{
		auto* LSD = UMuthMBPLib::GenStreamingLevelInstance(this, OnLoadSceneName, FVector::ZeroVector, FRotator::ZeroRotator);
		if (LSD)
		{
			GetWorld()->AddStreamingLevel(LSD);
		}
	}
	if (OnArrivedSceneName!="")
	{
		//Cached it to prevent freeze the game.
		CachedOnArrivedLevel = UMuthMBPLib::GenStreamingLevelInstance(this, OnArrivedSceneName, FVector::ZeroVector, FRotator::ZeroRotator);
		if (CachedOnArrivedLevel)
		{
			CachedOnArrivedLevel->bInitiallyVisible = false;
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

void ULoadScene::OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EEI)
{
	OnLoadSceneName = Args.Object->GetStringField("OnLoadSceneName");
	OnArrivedSceneName = Args.Object->GetStringField("OnArrivedSceneName");
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
	OnLoadSceneNameInfo->StrValue = OnLoadSceneName;
	DetailCategoryInto.ItemList.Add(OnLoadSceneNameInfo);

	TSharedPtr<FDetailItemString> OnArrivedSceneNameInfo = MakeShareable(new FDetailItemString());
	OnArrivedSceneNameInfo->DetailCallbackStr.BindUFunction(this, "OnStringDetailsCallback");
	OnArrivedSceneNameInfo->InstructionInstance = this;
	OnArrivedSceneNameInfo->Name = "OnArrivedSceneName";
	OnArrivedSceneNameInfo->DisplayName = LOCTEXT("OnArrivedSceneName", "OnArrivedSceneName");
	OnArrivedSceneNameInfo->StrValue = OnArrivedSceneName;
	DetailCategoryInto.ItemList.Add(OnArrivedSceneNameInfo);
	DetailsBuilder->AddCategory(DetailCategoryInto);
}

FBlueprintJsonObject ULoadScene::GenArgsJsonObject_Implementation()
{
	FBlueprintJsonObject BPJsonObj = Super::GenArgsJsonObject_Implementation();
	BPJsonObj.Object->SetStringField("OnLoadSceneName", OnLoadSceneName);
	BPJsonObj.Object->SetStringField("OnArrivedSceneName", OnArrivedSceneName);
	return BPJsonObj;
}

#undef LOCTEXT_NAMESPACE