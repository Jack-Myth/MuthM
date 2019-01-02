// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "LoadScene.h"
#include "JsonObject.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/World.h"

void ULoadScene::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	FString SceneName = Args.Object->GetStringField("Scene");
	if (SceneName!="")
	{
		bool bSuccessful;
		ULevelStreamingDynamic* LSD = ULevelStreamingDynamic::LoadLevelInstance(this, SceneName, FVector::ZeroVector, FRotator::ZeroRotator, bSuccessful);
		if (bSuccessful&&LSD) //Maybe only need one?
		{
			LSD->SetShouldBeLoaded(true);
			LSD->SetShouldBeVisible(true);
			GetWorld()->AddStreamingLevel(LSD);
			//GetWorld()->UpdateLevelStreaming();
		}
	}
	DestroySelf();
}
