// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailInputString.h"
#include "DetailStructures.h"

void UDetailInputString::FireOnValueChanged()
{
	_DetailItemInfo->DetailCallbackStr.ExecuteIfBound(_DetailItemInfo->InstructionInstance, _DetailItemInfo->Name, _DetailItemInfo->StrValue);
}

void UDetailInputString::ValueInit(TSharedPtr<struct FDetailItemString> DetailItemInfo)
{
	_DetailItemInfo = DetailItemInfo;
	K2_OnValueInit(_DetailItemInfo->DisplayName, _DetailItemInfo->InstructionInstance, _DetailItemInfo->StrValue);
}
