// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MusicManager.h"
#include "EditorMainUIBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UEditorMainUIBase : public UUserWidget
{
	GENERATED_BODY()

	//Current Time.
	float _TimePoint;
	class UEditorPanelBase* _EditorPanel;
public:
	UFUNCTION(BlueprintCallable)
		void UIEnterPIE();
	UFUNCTION(BlueprintCallable)
		void UIExitPIE();
	UFUNCTION(BlueprintCallable)
		void PlayMusic();
	UFUNCTION(BlueprintCallable)
		void PauseMusic();
	UFUNCTION(BlueprintImplementableEvent)
		void CollapseUIForEnterPIE();
	UFUNCTION(BlueprintImplementableEvent)
		void ExpandedUIOnExitPIE();
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionSelected(UInstruction* SelectedInstruction);
	UFUNCTION(BlueprintImplementableEvent)
		void OnFillMusicInfo(FMusicInfo MusicInfo);
	void Init(FMusicInfo MusicInfo,TScriptInterface<class IMMScript> MMScript);
	UFUNCTION(BlueprintImplementableEvent)
		void OnPlaceEditorPanel(class UEditorPanelBase* EditorPanel);
	void NativeOnFillBPMInfo(float BPM);
	UFUNCTION(BlueprintImplementableEvent)
		void OnFillBPMInfo(float BPM);
	UFUNCTION(BlueprintPure)
		class UEditorPanelBase* GetEditorPanel()
	{
		return _EditorPanel;
	}
};
