// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "SetSpeed.h"
#include "JsonObject.h"

void USetSpeed::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	double _Speed;
	if (Args.Object->TryGetNumberField("Speed", _Speed))
		this->Speed = _Speed;
	else
		this->Speed = 1;
}

void USetSpeed::OnPrepare_Implementation()
{
	SetGlobalNumberData("_MMSpeed", Speed);
	DestroySelf();
}
