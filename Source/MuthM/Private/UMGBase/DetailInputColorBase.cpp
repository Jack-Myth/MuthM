// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailInputColorBase.h"

FString UDetailInputColorBase::GetRGBAHexFromColor(const FLinearColor& Color) const
{
	return "#" + Color.ToFColor(false).ToHex();
}

FLinearColor UDetailInputColorBase::GetColorFromRGBAHex(const FString& RGBAHex) const
{
	return FColor::FromHex(RGBAHex).ReinterpretAsLinear();
}
