// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DownloadManager.h"
#include "DownloadManagerImpl.h"

// Add default functionality here for any IDownloadManager functions that are not pure virtual.

TScriptInterface<IDownloadManager> IDownloadManager::Get()
{
	return GetMutableDefault<UDownloadManagerImpl>();
}
