// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ObjectMacros.h"
#include "DetailStructures.generated.h"

UENUM()
enum class EDetailInputType :uint8
{
	DIT_String		UMETA(DisplayName = "String"),
	DIT_Number		UMETA(DisplayName = "Number"),
	DIT_Custom		UMETA(DisplayName = "Custom")
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDetailCallbackStr, class UInstruction*, InstructionInstance, FName, PropertyName, FString, StrValue);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDetailCallbackNumber, class UInstruction*, InstructionInstance, FName, PropertyName, float, NumberValue);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDetailCallbackCustom, class UInstruction*, InstructionInstance, FName, PropertyName, class UDetailInputCustomBase*, CustomWidget);

USTRUCT(BlueprintType,BlueprintInternalUseOnly)
struct FDetailItem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FName Name;
	UPROPERTY(BlueprintReadWrite)
		FText DisplayName;
	UPROPERTY(BlueprintReadWrite)
		class UInstruction* InstructionInstance;

	//Use to determine the type of input.
	//Just for read only,Do not change it.
	EDetailInputType InputType;
};

USTRUCT(BlueprintType)
struct FDetailItemString :public FDetailItem
{
	GENERATED_BODY()
public:
	FDetailItemString();
	UPROPERTY(BlueprintReadWrite)
		FString StrValue;
	UPROPERTY(BlueprintReadWrite)
		FDetailCallbackStr DetailCallbackStr;
};

USTRUCT(BlueprintType)
struct FDetailItemNumber :public FDetailItem
{
	GENERATED_BODY()
public:
	FDetailItemNumber();
	UPROPERTY(BlueprintReadWrite)
		float NumberValue;
	UPROPERTY(BlueprintReadWrite)
		FDetailCallbackNumber DetailCallbackNumber;
	UPROPERTY(BlueprintReadWrite)
		float SlideUnit = 0;
};

USTRUCT(BlueprintType)
struct FDetailItemCustom :public FDetailItem
{
	GENERATED_BODY()
public:
	FDetailItemCustom();
	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<UDetailInputCustomBase> CustomWidgetClass;
	UPROPERTY(BlueprintReadWrite)
		FDetailCallbackCustom DetailCallbackCustom;
};

USTRUCT(BlueprintType)
struct FDetailCategoryStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FName Title;
	UPROPERTY(BlueprintReadWrite)
		FText DisplayTitle;
	TArray<TSharedPtr<FDetailItem>> ItemList;
};