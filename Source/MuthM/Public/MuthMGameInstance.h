// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MusicManager.h"
#include "Engine/GameInstance.h"
#include "MuthMGameInstance.generated.h"

USTRUCT()
struct FGameArgs
{
	GENERATED_BODY()

	TSharedPtr<class FMDATFile> _MDAT; 
	//Relative to MDAT
	FString MMSFileName;
	FMusicInfo MainMusicInfo;
};

/**
 * 
 */
UCLASS()
class MUTHM_API UMuthMGameInstance : public UGameInstance
{
	GENERATED_BODY()
	UPROPERTY()
		FGameArgs _GameArgs;
public:
	UMuthMGameInstance();

	//Fill GameArgs for Level change
	void FillGameArgs(FGameArgs targetGameArgs);

	//Once the GameArgs is exchanged, the ref to object sush as _MDAT will be clear
	//It's designed to recycle the objects.
	FGameArgs ExchangeGameArgs();

	virtual void PostInitProperties() override;

	void OnApplicationDeactive();
	void OnApplicationReactive();
	void OnApplicationSwitchBackground();
	void OnApplicationSwitchForeground();
};
