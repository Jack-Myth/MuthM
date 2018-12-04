// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "QuestionBoxBase.h"

void UQuestionBoxBase::Yes()
{
	OnYes.Broadcast();
	RemoveFromParent();
}

void UQuestionBoxBase::No()
{
	OnNo.Broadcast();
	RemoveFromParent();
}

void UQuestionBoxBase::Cancel()
{
	OnCancel.Broadcast();
	RemoveFromParent();
}
