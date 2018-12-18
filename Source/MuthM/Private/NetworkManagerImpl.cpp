// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "NetworkManagerImpl.h"
#include "HttpModule.h"
#include "UserManager.h"

TSharedPtr<class IHttpRequest> UNetworkManagerImpl::GenRequest()
{
	TSharedPtr<class IHttpRequest> tmpRequest = FHttpModule::Get().CreateRequest();
	tmpRequest->SetHeader("Cookie", IUserManager::Get(this)->GenCookies());
	return tmpRequest;
}

void UNetworkManagerImpl::SimpleGetRequestJson(FString URL, FOnJsonRespond OnJsonRespond)
{
	//UNDONE: SimpleGetRequestJson()
}

void UNetworkManagerImpl::SimplePostRequestJson(FString URL, TMap<FString, FString> PostData, FOnJsonRespond OnJsonRespond)
{
	//UNDONE: SimpleGetRequestJson()
}
