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
	FString _CachedMDATFileName;
	EPlayType TargetPlayType =EPlayType::PT_Game;
protected:
	FORCEINLINE TScriptInterface<IMMScript> GetMainMMSInstance()
	{
		return _MainMMSInstance;
	}


	UFUNCTION()
		void OnMusicPositionCallback(TScriptInterface<IMainSoundWave> MainSoundWave, float PlaybackPercent);
	//This Function will also save the score to disk.
	void ShowGameResult();
	virtual void BindDelegates();
	void OnBackPressed();
	void ReturnToMainMenu();
public:
	UPROPERTY(BlueprintAssignable)
		FOnMusicPlaybackTimeUpdate OnMusicPlaybackTimeUpdate;
	UPROPERTY(BlueprintAssignable)
		FOnGameEnded OnGameEnded;
	UPROPERTY()  //Prevent Read By Blueprint(such as UserWidget)
		TMap<FName, float> GlobalDataNumber;
	UPROPERTY()
		TMap<FName, FString> GlobalDataString;
	UPROPERTY()
		TMap<FName, UObject*> GlobalDataObject;
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


	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;


	virtual void ReturnToMainMenuHost() override;

protected:
	virtual void BeginPlay() override;
};
