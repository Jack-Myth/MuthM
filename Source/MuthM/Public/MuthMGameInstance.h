// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "MuthMGameInstance.generated.h"

USTRUCT()
struct FGameArgs
{
	GENERATED_BODY()

	TSharedPtr<class FMDATFile> _MDAT; 
	//Relative to MDAT
	FString MMSFileName;
	FString MusicTitle;
	FString Musician;
};

/**
 * 
 */
UCLASS()
class MUTHM_API UMuthMGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()
	UPROPERTY()
		FGameArgs _GameArgs;
public:
	//Fill GameArgs for Level change
	void FillGameArgs(FGameArgs targetGameArgs);

	//Once the GameArgs is exchanged, the ref to object sush as _MDAT will be clear
	//It's designed to recycle the objects.
	FGameArgs ExchangeGameArgs();
};
