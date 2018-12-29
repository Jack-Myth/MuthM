// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MessageBoxBase.h"

void UMessageBoxBase::Confirm()
{
	OnMessageBoxConfirmed.Broadcast();
}
