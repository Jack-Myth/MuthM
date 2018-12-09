// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailStructures.h"

FDetailItemString::FDetailItemString()
{
	InputType = EDetailInputType::DIT_String;
}

FDetailItemNumber::FDetailItemNumber()
{
	InputType = EDetailInputType::DIT_Number;
}

FDetailItemCustom::FDetailItemCustom()
{
	InputType = EDetailInputType::DIT_Custom;
}
