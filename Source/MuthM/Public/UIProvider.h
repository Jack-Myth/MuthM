// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PauseUIBase.h"
#include "DetailsListBase.h"
#include "DetailCategoryBase.h"
#include "DetailInputNumberBase.h"
#include "DetailInputStringBase.h"
#include "EditorMainUIBase.h"
#include "EditorPanelBase.h"
#include "InstructionWidgetBase.h"
#include "GameUIBase.h"
#include "GameResultUIBase.h"
#include "WelcomeUIBase.h"
#include "MainMenuUIBase.h"
#include "MusicSelectionUIBase.h"
#include "UIProvider.generated.h"

#define MUTHM_UI_DEFINE(UINAME,DEFAULT_UI_REF) \
TSubclassOf<U##UINAME##Base> Get##UINAME##()\
{if(UINAME.IsBound()){\
TSubclassOf<U##UINAME##Base> _uclass; UINAME.Execute(_uclass); \
return _uclass;}else{return LoadClass<U##UINAME##Base>(NULL,TEXT(DEFAULT_UI_REF));}}

DECLARE_DYNAMIC_DELEGATE_OneParam(FPauseUIDelegate, TSubclassOf<UPauseUIBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDetailsListDelegate, TSubclassOf<UDetailsListBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDetailCategoryDelegate, TSubclassOf<UDetailCategoryBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDetailInputNumberDelegate, TSubclassOf<UDetailInputNumberBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDetailInputStringDelegate, TSubclassOf<UDetailInputStringBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FEditorMainUIDelegate, TSubclassOf<UEditorMainUIBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FEditorPanelDelegate, TSubclassOf<UEditorPanelBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FInstructionWidgetDelegate, TSubclassOf<UInstructionWidgetBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FGameUIDelegate, TSubclassOf<UGameUIBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FGameResultUIDelegate, TSubclassOf<UGameResultUIBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FWelcomeUIDelegate, TSubclassOf<UWelcomeUIBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FMainMenuUIDelegate, TSubclassOf<UMainMenuUIBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FMusicSelectionUIDelegate, TSubclassOf<UMusicSelectionUIBase>&, _returnV);

/**
 * 
 */
UCLASS(NotBlueprintable)
class MUTHM_API UUIProvider : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure,DisplayName="Get UI Provider")
	static UUIProvider* Get()
	{
		return GetMutableDefault<UUIProvider>();
	}

	UPROPERTY(BlueprintReadWrite)
		FPauseUIDelegate PauseUI;
	MUTHM_UI_DEFINE(PauseUI, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/PauseUI.PauseUI_C'")
	UFUNCTION(BlueprintCallable)
		void ClearPauseUI() { PauseUI.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FDetailCategoryDelegate DetailCategory;
	MUTHM_UI_DEFINE(DetailCategory, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/DetailCategory.DetailCategory_C'")
		UFUNCTION(BlueprintCallable)
		void ClearDetailCategory() { DetailCategory.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FDetailsListDelegate DetailsList;
	MUTHM_UI_DEFINE(DetailsList, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/DetailsList.DetailsList_C'")
		UFUNCTION(BlueprintCallable)
		void ClearDetailsList() { DetailsList.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FDetailInputNumberDelegate DetailInputNumber;
	MUTHM_UI_DEFINE(DetailInputNumber, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/DetailInputNumber.DetailInputNumber_C'")
		UFUNCTION(BlueprintCallable)
		void ClearDetailInputNumber() { DetailInputNumber.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FDetailInputStringDelegate DetailInputString;
	MUTHM_UI_DEFINE(DetailInputString, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/DetailInputString.DetailInputString_C'")
		UFUNCTION(BlueprintCallable)
		void ClearDetailInputString() { DetailInputString.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FEditorMainUIDelegate EditorMainUI;
	MUTHM_UI_DEFINE(EditorMainUI, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/EditorMainUI.EditorMainUI_C'")
		UFUNCTION(BlueprintCallable)
		void ClearEditorMainUI() { EditorMainUI.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FEditorPanelDelegate EditorPanel;
	MUTHM_UI_DEFINE(EditorPanel, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/EditorPanel.EditorPanel_C'")
		UFUNCTION(BlueprintCallable)
		void ClearEditorPanel() { EditorPanel.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FInstructionWidgetDelegate InstructionWidget;
	MUTHM_UI_DEFINE(InstructionWidget, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/InstructionWidget.InstructionWidget_C'")
		UFUNCTION(BlueprintCallable)
		void ClearInstructionWidget() { InstructionWidget.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FGameUIDelegate GameUI;
	MUTHM_UI_DEFINE(GameUI, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/GameUI.GameUI_C'")
		UFUNCTION(BlueprintCallable)
		void ClearGameUI() { GameUI.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FGameResultUIDelegate GameResultUI;
	MUTHM_UI_DEFINE(GameResultUI, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/GameResultUI.GameResultUI_C'")
		UFUNCTION(BlueprintCallable)
		void ClearGameResultUI() { GameResultUI.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FWelcomeUIDelegate WelcomeUI;
	MUTHM_UI_DEFINE(WelcomeUI, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/WelcomeUI.WelcomeUI_C'")
		UFUNCTION(BlueprintCallable)
		void ClearWelcomeUI() { WelcomeUI.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FMainMenuUIDelegate MainMenuUI;
	MUTHM_UI_DEFINE(MainMenuUI, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/MainMenuUI.MainMenuUI_C'")
		UFUNCTION(BlueprintCallable)
		void ClearMainMenuUI() { MainMenuUI.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		FMusicSelectionUIDelegate MusicSelectionUI;
	MUTHM_UI_DEFINE(MusicSelectionUI, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/MusicSelectionUI.MusicSelectionUI_C'")
		UFUNCTION(BlueprintCallable)
		void ClearMusicSelectionUI() { MusicSelectionUI.Unbind(); }
};
