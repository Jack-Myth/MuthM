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

USTRUCT(BlueprintType)
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
	EDetailInputType InputType;
};

USTRUCT(BlueprintType)
struct FDetailItemString :public FDetailItem
{
	GENERATED_BODY()
public:
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
	UPROPERTY(BlueprintReadWrite)
		float NumberValue;
	UPROPERTY(BlueprintReadWrite)
		FDetailCallbackNumber DetailCallbackNumber;
};

USTRUCT(BlueprintType)
struct FDetailItemCustom :public FDetailItem
{
	GENERATED_BODY()
public:
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
	TArray<TSharedPtr<FDetailItem>> ItemList;
};