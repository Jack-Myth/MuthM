// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "NetworkManager.h"
#include "NetworkManagerImpl.h"

// Add default functionality here for any INetworkManager functions that are not pure virtual.

TScriptInterface<INetworkManager> INetworkManager::Get()
{
	return GetMutableDefault<UNetworkManagerImpl>();
}
