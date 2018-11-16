// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicManager.h"
#include "MusicManagerImpl.h"


// Add default functionality here for any IMusicManager functions that are not pure virtual.

TScriptInterface<IMusicManager> IMusicManager::Get()
{
	return GetMutableDefault<UMusicManagerImpl>();
}
