// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailBuilder.h"
#include "DetailBuilderImpl.h"


// Add default functionality here for any IDetailBuilder functions that are not pure virtual.

TScriptInterface<IDetailBuilder> IDetailBuilder::GenNewBuilder()
{
	return NewObject<UDetailBuilderImpl>();
}
