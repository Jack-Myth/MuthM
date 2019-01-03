// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_TwoParams(FOnGetOpenFileName,bool, TArray<FString>);

class IGameHAL
{
public:
	//Filters is used for extensions.
	//Use Filters for desktop platform(Windows) and MIME will be auto detected on mobile (Android)
	//MIME will use Filters[0] to get MIME.
	virtual void OpenFileDialog(const FString& Title, const FString& DefaultPath, const TArray<FString>& Filters, bool AllowMultipleSelected, FOnGetOpenFileName Delegate) = 0;

};