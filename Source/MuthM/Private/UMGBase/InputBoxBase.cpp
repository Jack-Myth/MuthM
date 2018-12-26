// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InputBoxBase.h"

void UInputBoxBase::Input(const FString& InputString)
{
	OnInput.Broadcast(InputString);
}

void UInputBoxBase::Cancelled()
{
	OnCancelled.Broadcast();
}
