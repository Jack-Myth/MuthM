// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailsBuilder.h"
#include "DetailsBuilderImpl.h"


// Add default functionality here for any IDetailBuilder functions that are not pure virtual.

TScriptInterface<IDetailsBuilder> IDetailsBuilder::GenNewBuilder()
{
	return NewObject<UDetailsBuilderImpl>();
}
