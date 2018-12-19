// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGameHAL.h"

class WindowsHALImpl :public IGameHAL
{

public:
	virtual bool OpenFileDialog(const FString& Title, const FString& DefaultPath, const TArray<FString>& Filters, bool AllowMultipleSelected, TArray<FString> OpenFileName) override;

};

typedef WindowsHALImpl GameHALImpl;