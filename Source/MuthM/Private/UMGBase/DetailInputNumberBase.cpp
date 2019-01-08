// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailInputNumberBase.h"
#include "DetailStructures.h"

void UDetailInputNumberBase::FireOnValueChanged()
{
	_DetailItemInfo->DetailCallbackNumber.ExecuteIfBound(_DetailItemInfo->InstructionInstance, _DetailItemInfo->Name, GetInputNumber());
}

void UDetailInputNumberBase::ValueInit(TSharedPtr<FDetailItemNumber> DetailItemInfo)
{
	_DetailItemInfo = DetailItemInfo;
	K2_OnValueInit(_DetailItemInfo->DisplayName, _DetailItemInfo->InstructionInstance, _DetailItemInfo->NumberValue,_DetailItemInfo->SlideUnit);
}
