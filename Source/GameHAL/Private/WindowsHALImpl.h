// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGameHAL.h"

class WindowsHALImpl :public IGameHAL
{

public:
	virtual void OpenFileDialog(const FString& Title, const FString& DefaultPath, const TArray<FString>& Filters, bool AllowMultipleSelected, FOnGetOpenFileName Delegate) override;

};

typedef WindowsHALImpl GameHALImpl;