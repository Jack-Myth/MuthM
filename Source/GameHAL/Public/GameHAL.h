// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGameHAL.h"

struct GAMEHAL_API FGameHAL
{
private:
	static IGameHAL* _GameHALInstance;
public:
	static IGameHAL& Get();
};