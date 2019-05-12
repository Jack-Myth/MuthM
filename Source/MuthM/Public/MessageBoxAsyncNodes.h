// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "TextProperty.h"
#include "MessageBoxAsyncNodes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMsgBoxReplied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputBoxReplied,const FString&,InputString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenFileResult, const TArray<FString>&, OpenFileName);

/**
 * 
 */
UCLASS()
class MUTHM_API UPupopMessage : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	virtual void Activate() override;
	FText Message;
	FText Title;
public:
	UPROPERTY(BlueprintAssignable)
		FOnMsgBoxReplied Confirmed;
	UFUNCTION()
		void OnConfirmed();
	UFUNCTION(BlueprintCallable,meta=(BlueprintInternalUseOnly = "true",WorldContext="WorldContextObj"),Category="MuthM MessageBox")
		static UPupopMessage* PupopMessage(UObject* WorldContextObj,FText Message, FText Title=FText());
};

/**
 * 
 */
UCLASS()
class MUTHM_API UPupopQuestion : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
		virtual void Activate() override;
	FText Question;
	FText Title;
	bool AllowCancel;
public:
	UPROPERTY(BlueprintAssignable)
		FOnMsgBoxReplied Yes;
	UPROPERTY(BlueprintAssignable)
		FOnMsgBoxReplied No;
	UPROPERTY(BlueprintAssignable)
		FOnMsgBoxReplied Cancelled;
	UFUNCTION()
		void OnYes();
	UFUNCTION()
		void OnNo();
	UFUNCTION()
		void OnCancel();
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true",WorldContext = "WorldContextObj"), Category = "MuthM MessageBox")
		static UPupopQuestion* PupopQuestion(UObject* WorldContextObj, FText Question, FText Title = FText(),bool AllowCancel=true);
};

/**
 *
 */
UCLASS()
class MUTHM_API UPupopInput : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
		virtual void Activate() override;
	FText Message;
	FText Title;
	FString DefaultInput;
	bool AllowCancel;
public:
	UPROPERTY(BlueprintAssignable)
		FOnInputBoxReplied Input;
	UPROPERTY(BlueprintAssignable)
		FOnMsgBoxReplied Cancelled;
	UFUNCTION()
		void OnInput(const FString& InputString);
	UFUNCTION()
		void OnCancel();
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObj"), Category = "MuthM MessageBox")
		static UPupopInput* PupopInput(UObject* WorldContextObj, FText Message, FText Title = FText(),const FString& DefaultInput="", bool AllowCancel = true);
};

/**
 *
 */
UCLASS()
class MUTHM_API UPupopOpenFileDialog : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
		virtual void Activate() override;
	FText _Title;
	FString _InitDir;
	TArray<FString> _Extensions;
	bool _AllowMultuipleSelected;
public:
	UPROPERTY(BlueprintAssignable)
		FOnOpenFileResult FileOpened;
	UPROPERTY(BlueprintAssignable)
		FOnMsgBoxReplied Cancelled;
	void OnFileReult(bool IsFileOpened,TArray<FString> FileName);
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObj"), Category = "MuthM MessageBox")
		static UPupopOpenFileDialog* PupopOpenFileDialog(UObject* WorldContextObj, FText Title, FString InitDir, TArray<FString> Extensions, bool AllowMultipleSelected = false);
};