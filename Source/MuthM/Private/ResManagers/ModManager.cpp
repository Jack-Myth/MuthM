// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ModManager.h"
#include "ModManagerImpl.h"

// Add default functionality here for any IModManager functions that are not pure virtual.

TScriptInterface<IModManager> IModManager::Get()
{
	return GetMutableDefault<UModManagerImpl>();
}
