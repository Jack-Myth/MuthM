// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailsListBase.h"

void UDetailsListBase::CloseDetails()
{
	OnDetailsListClosed.Broadcast(this);
	OnDetailsClosed();
}
