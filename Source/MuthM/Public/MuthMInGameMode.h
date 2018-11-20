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
	UPROPERTY()
		class UAudioComponent* _MainSoundComponent;

	//Because the Playback time will be wrong in some platform (Android for example)
	//So calculate playback time by Tick() and control pause and resume by LifeCycleComponent;
	float MusicPlaybackTime = 0;
	UPROPERTY()
		class UPauseUIBase* pPauseUI=nullptr;
public:
	AMuthMInGameMode();
	//GetScoreCore
	FORCEINLINE TSharedPtr<class FMDATFile> GetMDAT()
	{
		return _pMDAT;
	}

	virtual void Tick(float DeltaSeconds) override;

	virtual void PauseGame() override;
	virtual void ResumeGame() override;
	void RestartGame();
protected:
	virtual void BeginPlay() override;
};
