// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "UserManager.h"
#include "UserManagerImpl.h"

// Add default functionality here for any IUserManager functions that are not pure virtual.

TScriptInterface<IUserManager> IUserManager::Get()
{
	return GetMutableDefault<UUserManagerImpl>();
}
