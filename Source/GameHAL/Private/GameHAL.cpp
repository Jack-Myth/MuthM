// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "GameHAL.h"
#include "ModuleManager.h"

#if PLATFORM_WINDOWS
#include "WindowsHALImpl.h"
#elif PLATFORM_ANDROID
#include "AndroidHALImpl.h"
#endif

IMPLEMENT_MODULE(FDefaultModuleImpl, GameHAL)

IGameHAL* FGameHAL::_GameHALInstance;

IGameHAL& FGameHAL::Get()
{
	if (!_GameHALInstance)
		_GameHALInstance = new GameHALImpl();
	return *_GameHALInstance;
}