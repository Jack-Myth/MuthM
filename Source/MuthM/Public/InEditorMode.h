// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MMScript.h"
#include "InGameMode.h"
#include "InEditorMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMInEditorMode, Log, All)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchPIE);

/**
 * 
 */
UCLASS()
class MUTHM_API AInEditorMode : public AInGameMode
{
	GENERATED_BODY()

	UPROPERTY()
		class UEditorMainUIBase* EditorMainUI;
	UPROPERTY()
		TScriptInterface<IMMScript> _EditorMMSInstance;
	class UWorld* _PIEWorld=nullptr;
protected:
	virtual void BeginPlay() override;
	virtual bool GenPIEEnvironment(class UWorld*& PIEWorld);
public:
	UPROPERTY(BlueprintAssignable)
		FOnSwitchPIE OnEnterPIE;
	UPROPERTY(BlueprintAssignable)
		FOnSwitchPIE OnExitPIE;
	UFUNCTION(BlueprintCallable)
		void EnterPIE();
	UFUNCTION(BlueprintCallable)
		void ExitPIE();
	UFUNCTION(BlueprintCallable)
		void Save();
	void PlayMusicOnly(float BeginTime);
	void PauseMusicOnly();
	UFUNCTION(BlueprintPure)
	class UEditorMainUIBase* GetEditorMainUI() const
	{
		return EditorMainUI;
	}
	TScriptInterface<IMMScript> GetEditorMMS()
	{
		return _EditorMMSInstance;
	}
	virtual void NativeOnGameEnded(EGameEndReason GameEndReason) override;
	TArray<class UTexture2D*> DrawMainMusicSpectrum(float BeginTime, float EndTime, uint32 ResTime, int32 ResFrequency);
	void SetMusicPlaySpeed(float PlaySpeed);
};
