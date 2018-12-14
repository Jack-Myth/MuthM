// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "NetworkManagerImpl.h"
#include "HttpModule.h"

TSharedPtr<class IHttpRequest> UNetworkManagerImpl::GenRequest()
{
	return FHttpModule::Get().CreateRequest();
}

void UNetworkManagerImpl::SimpleGetRequestJson(FString URL, FOnJsonRespond OnJsonRespond)
{

}

void UNetworkManagerImpl::SimplePostRequestJson(FString URL, TMap<FString, FString> PostData, FOnJsonRespond OnJsonRespond)
{

}
