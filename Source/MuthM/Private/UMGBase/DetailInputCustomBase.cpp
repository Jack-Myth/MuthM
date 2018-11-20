// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailInputCustomBase.h"
#include "DetailStructures.h"

void UDetailInputCustomBase::FireOnValueChanged()
{
	_DetailItemInfo->DetailCallbackCustom.ExecuteIfBound(_DetailItemInfo->InstructionInstance, _DetailItemInfo->Name, this);
}

void UDetailInputCustomBase::ValueInit(TSharedPtr<FDetailItemCustom> DetailItemInfo)
{
	_DetailItemInfo = DetailItemInfo;
	K2_OnValueInit(DetailItemInfo->DisplayName, DetailItemInfo->InstructionInstance);
}
