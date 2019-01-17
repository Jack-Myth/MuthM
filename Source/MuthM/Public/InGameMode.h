// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MuthMGameModeBase.h"
#include "MMScript.h"
#include "MusicManager.h"
#include "InGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMInGameMode, Log, All);

UENUM(BlueprintType)
enum class EGameEndReason :uint8
{
	GER_GameFinished	UMETA(DisplayName="GameFinished"),
	GER_Restart			UMETA(DisplayName="Restart"),
	GER_ExitPIE			UMETA(DisplayName="ExitPIE"),
	GER_Return			UMETA(DisplayName="Return")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMusicPlaybackTimeUpdate, float,CurrentTime, float,Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnded, EGameEndReason, EndReason);

/**
 * 
 */
UCLASS()
class MUTHM_API AInGameMode : public AMuthMGameModeBase
{
	GENERATED_BODY()



	FTimerHandle DelayTimerHandle;
	float DelayCounter = 0;
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
	float _GameTime = 0;
	FString _MMSFileName;
	//The two cached variable is prepared for RestartGame.
	FMusicInfo _CachedMusicInfo;
	TArray<uint8> _CachedMMSData;
protected:
	FORCEINLINE TScriptInterface<IMMScript> GetMainMMSInstance()
	{
		return _MainMMSInstance;
	}


	UFUNCTION()
		void OnMusicPositionCallback(TScriptInterface<IMainSoundWave> MainSoundWave, float PlaybackPercent);

public:
	UPROPERTY(BlueprintAssignable)
		FOnMusicPlaybackTimeUpdate OnMusicPlaybackTimeUpdate;
	UPROPERTY(BlueprintAssignable)
		FOnGameEnded OnGameEnded;
	UPROPERTY(BlueprintReadWrite)
		TMap<FName, float> GlobalDataNumber; 
	AInGameMode();
	FORCEINLINE TSharedPtr<class FMDATFile> GetMDAT()
	{
		return _pMDAT;
	}
	FORCEINLINE FMusicInfo GetMusicInfo()
	{
		return _CachedMusicInfo;
	}
	void StartGame(FMusicInfo MusicInfo, const TArray<uint8>& MMSData);
	virtual void PauseGame() override;
	virtual void ResumeGame() override;
	void RestartGame();

	//Unload All Map, and Call delegate.
	void StopGame();
	virtual void NativeOnGameEnded(EGameEndReason GameEndReason);
	UFUNCTION(BlueprintPure)
		FORCEINLINE TScriptInterface<class IMainSoundWave> GetGameMainMusic()
	{
		return _GameMainMusic;
	}
	//This Function will also save the score to disk.
	void ShowGameResult();
	void ReturnToMainMenu();

	UFUNCTION(BlueprintPure)
		FORCEINLINE TScriptInterface<IMMScript> GetGameMMScript() const
	{
		return _MainMMSInstance;
	}

	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetGameTime()const
	{
		return _GameTime;
	}
protected:
	virtual void BeginPlay() override;
};
