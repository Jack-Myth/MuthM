// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailInputStringBase.h"
#include "DetailStructures.h"

void UDetailInputStringBase::FireOnValueChanged()
{
	_DetailItemInfo->DetailCallbackStr.ExecuteIfBound(_DetailItemInfo->InstructionInstance, _DetailItemInfo->Name, GetInputString());
}

void UDetailInputStringBase::ValueInit(TSharedPtr<struct FDetailItemString> DetailItemInfo)
{
	_DetailItemInfo = DetailItemInfo;
	K2_OnValueInit(_DetailItemInfo->DisplayName, _DetailItemInfo->InstructionInstance, _DetailItemInfo->StrValue);
}
