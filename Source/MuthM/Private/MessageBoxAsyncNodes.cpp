// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MessageBoxAsyncNodes.h"
#include "UIProvider.h"
#include "QuestionBoxBase.h"
#include "MessageBoxBase.h"

void UPupopMessage::Activate()
{
	auto MsgBoxClass = UUIProvider::Get()->GetMessageBox();
	UMessageBoxBase* MsgBoxUI = Cast<UMessageBoxBase>(UUserWidget::CreateWidgetInstance(*(_WorldContext->GetWorld()),MsgBoxClass,"MsgBox"));
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
	UPupopMessage* popMsg = NewObject<UPupopMessage>();
	popMsg->Message = Message;
	popMsg->Title = Title;
	popMsg->_WorldContext = WorldContextObj;
	return popMsg;
}

void UPupopQuestion::Activate()
{
	auto QuestionBoxClass = UUIProvider::Get()->GetQuestionBox();
	auto* QuestionBoxUI = Cast<UQuestionBoxBase>(UUserWidget::CreateWidgetInstance(*(_WorldContext->GetWorld()), QuestionBoxClass, "QuestionBox"));
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
	auto* popQuestion = NewObject<UPupopQuestion>();
	popQuestion->Question= Question;
	popQuestion->Title = Title;
	popQuestion->AllowCancel = AllowCancel;
	popQuestion->_WorldContext = WorldContextObj;
	return popQuestion;
}
