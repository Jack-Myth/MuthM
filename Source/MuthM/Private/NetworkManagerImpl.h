// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NetworkManager.h"
#include "NetworkManagerImpl.generated.h"

/**
 * 
 */
UCLASS()
class UNetworkManagerImpl : public UObject,public INetworkManager
{
	GENERATED_BODY()

	void RespondHandler();
public:
	virtual TSharedPtr<class IHttpRequest> GenRequest() override;
	virtual void SimpleGetRequestJson(FString URL, FOnJsonRespond OnJsonRespond) override;
	virtual void SimplePostRequestJson(FString URL, TMap<FString, FString> PostData, FOnJsonRespond OnJsonRespond) override;

};
