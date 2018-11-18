// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailStructures.h"

UDetailItem* UDetailItemString::GenDetailItemString(FName ItemTitle, FString StrValue, FDetailCallbackStr DetailCallbackStr)
{
	auto* tmpThis = NewObject<UDetailItemString>();
	tmpThis->Title = ItemTitle;
	tmpThis->StrValue = StrValue;
	tmpThis->DetailCallbackStr = DetailCallbackStr;
	return tmpThis;
}

UDetailItem* UDetailItemNumber::GenDetailItemNumber(FName ItemTitle, float NumberValue, FDetailCallbackNumber DetailCallbackNumber)
{
	auto* tmpThis = NewObject<UDetailItemNumber>();
	tmpThis->Title = ItemTitle;
	tmpThis->NumberValue = NumberValue;
	tmpThis->DetailCallbackNumber = DetailCallbackNumber;
	return tmpThis;
}

UDetailItem* UDetailItemCustom::GenDetailItemCustom(FName ItemTitle, class UUserWidget* CustomWidget, FDetailCallbackCustom DetailCallbackCustom)
{
	auto* tmpThis = NewObject<UDetailItemCustom>();
	tmpThis->Title = ItemTitle;
	tmpThis->CustomWidget=CustomWidget;
	tmpThis->DetailCallbackCustom = DetailCallbackCustom;
	return tmpThis;
}
