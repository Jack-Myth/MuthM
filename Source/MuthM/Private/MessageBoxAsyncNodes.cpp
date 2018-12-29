// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MessageBoxAsyncNodes.h"
#include "UIProvider.h"
#include "QuestionBoxBase.h"
#include "MessageBoxBase.h"
#include "InputBoxBase.h"

void UPupopMessage::Activate()
{
	auto MsgBoxClass = UUIProvider::Get(this)->GetMessageBox();
	UMessageBoxBase* MsgBoxUI = Cast<UMessageBoxBase>(UUserWidget::CreateWidgetInstance(*GetWorld(),MsgBoxClass, NAME_None));
	MsgBoxUI->OnMessageBoxConfirmed.AddDynamic(this,&UPupopMessage::OnConfirmed);
	MsgBoxUI->AddToViewport(900);
	MsgBoxUI->OnPupopMessage(Message, Title);
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
	auto* QuestionBoxUI = Cast<UQuestionBoxBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), QuestionBoxClass, NAME_None));
	QuestionBoxUI->OnYes.AddDynamic(this, &UPupopQuestion::OnYes);
	QuestionBoxUI->OnNo.AddDynamic(this, &UPupopQuestion::OnNo);
	QuestionBoxUI->OnCancel.AddDynamic(this, &UPupopQuestion::OnCancel);
	QuestionBoxUI->AddToViewport(900);
	QuestionBoxUI->OnPupopQuestion(Question, Title, AllowCancel);
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

void UPupopQuestion::OnCancel()
{
	Cancelled.Broadcast();
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

void UPupopInput::Activate()
{
	auto InputBoxClass = UUIProvider::Get(this)->GetInputBox();
	auto* InputBoxUI = Cast<UInputBoxBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), InputBoxClass, NAME_None));
	InputBoxUI->OnInput.AddDynamic(this, &UPupopInput::OnInput);
	InputBoxUI->OnCancelled.AddDynamic(this, &UPupopInput::OnCancel);
	InputBoxUI->AddToViewport(900);
	InputBoxUI->OnPupopInput(Title, Message, DefaultInput, AllowCancel);
}

void UPupopInput::OnInput(const FString& InputString)
{
	Input.Broadcast(InputString);
	SetReadyToDestroy();
}

void UPupopInput::OnCancel()
{
	Cancelled.Broadcast();
	SetReadyToDestroy();
}

UPupopInput* UPupopInput::PupopInput(UObject* WorldContextObj, FText Message, FText Title /*= FText()*/, const FString& DefaultInput/*=""*/, bool AllowCancel /*= true*/)
{
	auto* popInput = NewObject<UPupopInput>(WorldContextObj);
	popInput->Message = Message;
	popInput->Title = Title;
	popInput->DefaultInput = DefaultInput;
	popInput->AllowCancel = AllowCancel;
	return popInput;
}
