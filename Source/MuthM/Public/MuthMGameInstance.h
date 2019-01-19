// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MusicManager.h"
#include "Engine/GameInstance.h"
#include "ResManagers/GlobalSaveGame.h"
#include "InstructionManager.h"
#include "MuthMGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMGameInstance, Log, All)

USTRUCT()
struct FGameArgs
{
	GENERATED_BODY()

	TSharedPtr<class FMDATFile> _MDAT; 
	//Relative to MDAT
	FString MMSFileName;
	FMusicInfo MainMusicInfo;
	FString MDATFilePath;
	int ScoreIndex = 0;
	bool bIsEditorMode = false;
};

struct FMuthMPIEInfo
{
	struct FWorldContext* GameWorldContext = nullptr;
	TSharedPtr<class SViewport> GameViewportWidget = nullptr;
	struct FWorldContext* PIEWorldContext=nullptr;
	TSharedPtr<class SViewport> PIEViewportWidget=nullptr;
	TSharedPtr<class FSceneViewport> PIEViewport = nullptr;
	TSharedPtr<class SWidget> GameWidgetRoot;
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
	TWeakPtr<class UGlobalSaveGame> GlobalSaveGame;
	TArray<FInstructionRef> CachedInstructionRef;
	TSharedPtr<FMuthMPIEInfo> PIESession=nullptr;
public:
	//Hold Reference of Managers and UIProvider.
	UPROPERTY()
		TScriptInterface<class IInstructionManager> InstructionManager;
	UPROPERTY()
		TScriptInterface<class IMusicManager> MusicManager;
	UPROPERTY()
		TScriptInterface<class INetworkManager> NetworkManager;
	UPROPERTY()
		TScriptInterface<class IDownloadManager> DownloadManager;
	UPROPERTY()
		TScriptInterface<class IModManager> ModManager;
	UPROPERTY()
		TScriptInterface<class IUserManager> UserManager;
	UPROPERTY()
		class UUIProvider* UIProvider;

	UMuthMGameInstance();

	//Fill GameArgs for Level change
	void FillGameArgs(FGameArgs targetGameArgs);

	//Once the GameArgs is exchanged, the ref to object sush as _MDAT will be clear
	//It's designed to recycle the objects.
	//but the 
	FGameArgs ExchangeGameArgs();

	virtual void PostInitProperties() override;

	void GetLastScoreInfo(FString& MDATFilePath, int& ScoreIndex,bool& IsEditorMode);

	void OnApplicationDeactive();
	void OnApplicationReactive();
	void OnApplicationSwitchBackground();
	void OnApplicationSwitchForeground();

	TSharedPtr<class UGlobalSaveGame> GetGlobalSaveGame();
	void SaveGlobalSaveGame();

	virtual void Init() override;

	void EnterPIEMode(class UWorld* PIEWorld);
	void ExitPIEMode();
};
