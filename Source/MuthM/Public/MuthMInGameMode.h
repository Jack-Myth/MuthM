// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MuthMGameModeBase.h"
#include "MMScript.h"
#include "MuthMInGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMInGameMode, Log, All)

/**
 * 
 */
UCLASS()
class MUTHM_API AMuthMInGameMode : public AMuthMGameModeBase
{
	GENERATED_BODY()

	TArray<TScriptInterface<IMMScript>> MainMMSInstance;
	TSharedPtr<class FMDATFile> _pMDAT;
	TScriptInterface<IMMScript> _MainMMSInstance;
	FString MusicTitle;
	FString Author;
	UPROPERTY()
		class USoundWave* _GameMainMusic;
public:
	//GetScoreCore
	FORCEINLINE TSharedPtr<class FMDATFile> GetMDAT()
	{
		return _pMDAT;
	}

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
};
