// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "GenericSelectionItemBase.h"

void UGenericSelectionItemBase::NotifySelected()
{
	OnSelected.Broadcast(this,GetDataIndex());
}
