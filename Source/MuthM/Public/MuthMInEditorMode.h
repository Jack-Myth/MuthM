// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MMScript.h"
#include "MuthMInGameMode.h"
#include "MuthMInEditorMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMInEditorMode, Log, All)

/**
 * 
 */
UCLASS()
class MUTHM_API AMuthMInEditorMode : public AMuthMInGameMode
{
	GENERATED_BODY()

	UPROPERTY()
		class UEditorMainUIBase* EditorMainUI;
	UPROPERTY()
		TScriptInterface<IMMScript> _EditorMMSInstance;
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "For EditorMainUI,recommend to use UIEnterPIE() instead."))
		void EnterPIE(float BeginTime);
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "For EditorMainUI,recommend to use UIExitPIE() instead."))
		void ExitPIE();
	void PlayMusicOnly(float BeginTime);
	void PauseMusicOnly();
	UFUNCTION(BlueprintCallable)
	class UEditorMainUIBase* GetEditorMainUI()
	{
		return EditorMainUI;
	}
};
