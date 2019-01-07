// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MuthMGameModeBase.h"
#include "MMScript.h"
#include "MusicManager.h"
#include "MuthMInGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMInGameMode, Log, All);

UENUM(BlueprintType)
enum class FGameEndReason :uint8
{
	GER_GameFinished	UMETA(DisplayName="GameFinished"),
	GER_Restart			UMETA(DisplayName="Restart"),
	GER_ExitPIE			UMETA(DisplayName="ExitPIE"),
	GER_Return			UMETA(DisplayName="Return")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicPlaybackTimeUpdate, float,CurrentTime, float,Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnded, FGameEndReason, EndReason);

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
		TScriptInterface<class IMainSoundWave> _GameMainMusic;
	UPROPERTY()
		TScriptInterface<class IMainSWPlayer> _MainSoundComponent;
	UPROPERTY()
		class UPauseUIBase* pPauseUI=nullptr;
	UPROPERTY()
		class UGameUIBase* _MainGameUI;
	UPROPERTY()
		class UScoreCore* _ScoreCore;
	//The two cached variable is prepared for RestartGame.
	FMusicInfo _CachedMusicInfo;
	TArray<uint8> _CachedMMSData;
protected:
	FORCEINLINE TScriptInterface<IMMScript> GetMainMMSInstance()
	{
		return _MainMMSInstance;
	}

	//Because the Playback time will be wrong in some platform (Android for example)
	//So calculate playback time by Tick() and control pause and resume by LifeCycleComponent;
	float MusicPlaybackTime = 0;
public:
	UPROPERTY(BlueprintAssignable)
		FOnMusicPlaybackTimeUpdate OnMusicPlaybackTimeUpdate;
	UPROPERTY(BlueprintAssignable)
		FOnGameEnded OnGameEnded;
	UPROPERTY()
		TMap<FName, float> GlobalDataNumber; 
	AMuthMInGameMode();
	//GetScoreCore
	UFUNCTION(BlueprintPure)
		FORCEINLINE class UScoreCore* GetScoreCore()
	{
		return _ScoreCore;
	}
	FORCEINLINE TSharedPtr<class FMDATFile> GetMDAT()
	{
		return _pMDAT;
	}
	FORCEINLINE FMusicInfo GetMusicInfo()
	{
		return _CachedMusicInfo;
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
	virtual void NativeOnGameEnded(FGameEndReason GameEndReason);
	UFUNCTION(BlueprintPure)
		FORCEINLINE TScriptInterface<class IMainSoundWave> GetGameMainMusic()
	{
		return _GameMainMusic;
	}
	//This Function will also save the score to disk.
	void ShowGameResult();
	void ReturnToMainMenu();
protected:
	virtual void BeginPlay() override;
};
