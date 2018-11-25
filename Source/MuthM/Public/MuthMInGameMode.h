// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MuthMGameModeBase.h"
#include "MMScript.h"
#include "MusicManager.h"
#include "MuthMInGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMInGameMode, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicPlaybackTimeUpdate, float,CurrentTime, float,Duration);

/**
 * 
 */
UCLASS()
class MUTHM_API AMuthMInGameMode : public AMuthMGameModeBase
{
	GENERATED_BODY()

protected:
	TSharedPtr<class FMDATFile> _pMDAT;
	TScriptInterface<IMMScript> _MainMMSInstance;
	UPROPERTY()
		class USoundWave* _GameMainMusic;
	UPROPERTY()
		class UAudioComponent* _MainSoundComponent;
	//Because the Playback time will be wrong in some platform (Android for example)
	//So calculate playback time by Tick() and control pause and resume by LifeCycleComponent;
	float MusicPlaybackTime = 0;
	UPROPERTY()
		class UPauseUIBase* pPauseUI=nullptr;
	UPROPERTY()
		class UGameUIBase* _MainGameUI;
	UPROPERTY()
		class UScoreCore* _ScoreCore;

	//The two cached variable is prepared for RestartGame.
	FMusicInfo _CachedMusicInfo;
	TArray<uint8> _CachedMMSData;
public:
	UPROPERTY(BlueprintAssignable)
		FOnMusicPlaybackTimeUpdate OnMusicPlaybackTimeUpdate;
	AMuthMInGameMode();
	//GetScoreCore
	UFUNCTION(BlueprintPure)
		class UScoreCore* GetScoreCore();
	FORCEINLINE TSharedPtr<class FMDATFile> GetMDAT()
	{
		return _pMDAT;
	}
	virtual void Tick(float DeltaSeconds) override;
	void StartGame(FMusicInfo MusicInfo, const TArray<uint8>& MMSData,float BeginTime);
	FORCEINLINE void StartGame(FMusicInfo MusicInfo, const TArray<uint8>& MMSData)
	{
		StartGame(MusicInfo, MMSData, 0);
	}
	virtual void PauseGame() override;
	virtual void ResumeGame() override;
	void RestartGame();
	void StopGame();

	//Notice:This function only draw gray map to RenderTarget2D
	void DrawMainMusicSpectrum(class UTextureRenderTarget2D* RenderTarget2D, float BeginTime, float EndTime,uint32 ResTime,int32 ResFrequency);
protected:
	virtual void BeginPlay() override;
};
