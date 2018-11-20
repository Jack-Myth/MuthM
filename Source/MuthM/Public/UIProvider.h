// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PauseUIBase.h"
#include "DetailCategory.h"
#include "UIProvider.generated.h"

#define MUTHM_UI_DEFINE(UINAME,UICLASS,DEFAULT_UI_REF) \
TSubclassOf<UICLASS> Get##UINAME##()\
{if(UINAME.IsBound()){\
TSubclassOf<UICLASS> _uclass; UINAME.Execute(_uclass); \
return _uclass;}else{return LoadClass<UICLASS>(NULL,TEXT(DEFAULT_UI_REF));}}

DECLARE_DYNAMIC_DELEGATE_OneParam(FPauseUIDelegate, TSubclassOf<UPauseUIBase>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDetailCategoryDelegate, TSubclassOf<UDetailCategory>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDetailInputCustom, TSubclassOf<UDetailInputCustom>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDetailInputNumberDelegate, TSubclassOf<UDetailInputNumber>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDetailInputStringDelegate, TSubclassOf<UDetailInputString>&, _returnV);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDetailsListDelegate, TSubclassOf<UDetailsList>&, _returnV);

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
	MUTHM_UI_DEFINE(PauseUI, UPauseUIBase, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/PauseUI.PauseUI_C'")
	UFUNCTION(BlueprintCallable)
		void ClearPauseUI() { PauseUI.Unbind(); }

	UPROPERTY(BlueprintReadWrite)
		UDetailCategory DetailCategory;
	MUTHM_UI_DEFINE(DetailCategory, UDetailCategory, "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/PauseUI.PauseUI_C'")
		UFUNCTION(BlueprintCallable)
		void ClearPauseUI() { PauseUI.Unbind(); }

};
