// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "QuestionBoxBase.h"

void UQuestionBoxBase::Yes()
{
	OnYes.Broadcast();
}

void UQuestionBoxBase::No()
{
	OnNo.Broadcast();
}

void UQuestionBoxBase::Cancel()
{
	OnCancel.Broadcast();
}
