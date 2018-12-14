// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BlueprintJsonLibrary.h"
#include "NetworkManager.generated.h"

#define MUTHM_URL_ROOT "https://do.jackmyth.cn/MuthM"

UINTERFACE(MinimalAPI,meta=(CannotImplementInterfaceInBlueprint))
class UNetworkManager : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnJsonRespond, int, ErrCode, FString, ErrMsg, FBlueprintJsonObject, JsonObj);

/**
 * 
 */
class MUTHM_API INetworkManager
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	static TScriptInterface<INetworkManager> Get(const UObject* WorldContextObj);
	virtual TSharedPtr<class IHttpRequest> GenRequest() = 0;
	UFUNCTION(BlueprintCallable)
		virtual void SimpleGetRequestJson(FString URL, FOnJsonRespond OnJsonRespond) = 0;
	UFUNCTION(BlueprintCallable)
		virtual void SimplePostRequestJson(FString URL, TMap<FString, FString> PostData, FOnJsonRespond OnJsonRespond) = 0;
};
