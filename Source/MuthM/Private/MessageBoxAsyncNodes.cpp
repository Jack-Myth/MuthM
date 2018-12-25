// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MessageBoxAsyncNodes.h"
#include "UIProvider.h"
#include "QuestionBoxBase.h"
#include "MessageBoxBase.h"

void UPupopMessage::Activate()
{
	auto MsgBoxClass = UUIProvider::Get(this)->GetMessageBox();
	UMessageBoxBase* MsgBoxUI = Cast<UMessageBoxBase>(UUserWidget::CreateWidgetInstance(*GetWorld(),MsgBoxClass,"MsgBox"));
	MsgBoxUI->OnMessageBoxConfirmed.AddDynamic(this,&UPupopMessage::OnConfirmed);
	MsgBoxUI->OnPupopMessage(Message, Title);
	MsgBoxUI->AddToViewport(900);
}

void UPupopMessage::OnConfirmed()
{
	Confirmed.Broadcast();
	SetReadyToDestroy();
}

UPupopMessage* UPupopMessage::PupopMessage(UObject* WorldContextObj, FText Message, FText Title/*=FText()*/)
{
	UPupopMessage* popMsg = NewObject<UPupopMessage>(WorldContextObj);
	popMsg->Message = Message;
	popMsg->Title = Title;
	return popMsg;
}

void UPupopQuestion::Activate()
{
	auto QuestionBoxClass = UUIProvider::Get(this)->GetQuestionBox();
	auto* QuestionBoxUI = Cast<UQuestionBoxBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), QuestionBoxClass, "QuestionBox"));
	QuestionBoxUI->OnYes.AddDynamic(this, &UPupopQuestion::OnYes);
	QuestionBoxUI->OnNo.AddDynamic(this, &UPupopQuestion::OnNo);
	QuestionBoxUI->OnPupopQuestion(Question, Title,AllowCancel);
	QuestionBoxUI->AddToViewport(900);
}

void UPupopQuestion::OnYes()
{
	Yes.Broadcast();
	SetReadyToDestroy();
}

void UPupopQuestion::OnNo()
{
	No.Broadcast();
	SetReadyToDestroy();
}

UPupopQuestion* UPupopQuestion::PupopQuestion(UObject* WorldContextObj, FText Question, FText Title /*= FText()*/,bool AllowCancel /*=true*/)
{
	auto* popQuestion = NewObject<UPupopQuestion>(WorldContextObj);
	popQuestion->Question= Question;
	popQuestion->Title = Title;
	popQuestion->AllowCancel = AllowCancel;
	return popQuestion;
}
