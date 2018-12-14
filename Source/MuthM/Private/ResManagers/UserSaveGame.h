// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UserSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class UUserSaveGame : public USaveGame
{
	GENERATED_BODY()

	UPROPERTY()
		int UserID;
	UPROPERTY()
		FString Token;
	UPROPERTY()
		TArray<FString> LoginNameCollection;
};
