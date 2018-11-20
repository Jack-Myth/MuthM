// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailInputCustom.h"
#include "DetailStructures.h"

void UDetailInputCustom::FireOnValueChanged()
{
	_DetailItemInfo->DetailCallbackCustom.ExecuteIfBound(_DetailItemInfo->InstructionInstance, _DetailItemInfo->Name, this);
}

void UDetailInputCustom::ValueInit(TSharedPtr<FDetailItemCustom> DetailItemInfo)
{
	_DetailItemInfo = DetailItemInfo;
	K2_OnValueInit(DetailItemInfo->DisplayName, DetailItemInfo->InstructionInstance);
}
