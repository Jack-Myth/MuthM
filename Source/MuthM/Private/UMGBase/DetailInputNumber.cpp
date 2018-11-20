// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailInputNumber.h"
#include "DetailStructures.h"

void UDetailInputNumber::FireOnValueChanged()
{
	_DetailItemInfo->DetailCallbackNumber.ExecuteIfBound(_DetailItemInfo->InstructionInstance, _DetailItemInfo->Name, GetInputNumber());
}

void UDetailInputNumber::ValueInit(TSharedPtr<FDetailItemNumber> DetailItemInfo)
{
	_DetailItemInfo = DetailItemInfo;
	K2_OnValueInit(_DetailItemInfo->DisplayName, _DetailItemInfo->InstructionInstance, _DetailItemInfo->NumberValue);
}
