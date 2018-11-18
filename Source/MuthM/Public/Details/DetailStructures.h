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
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDetailCallbackCustom, class UInstruction*, InstructionInstance, FName, PropertyName, class UUserWidget*, CustomWidget);

UCLASS(MinimalAPI,NotBlueprintable, BlueprintType)
class UDetailItem :public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FName Title;
};

UCLASS(MinimalAPI,NotBlueprintType, NotBlueprintable)
class UDetailItemString :public UDetailItem
{
	GENERATED_BODY()
public:
	FString StrValue;
	FDetailCallbackStr DetailCallbackStr;
	UFUNCTION(BlueprintPure)
		static UDetailItem* GenDetailItemString(FName ItemTitle, FString StrValue, FDetailCallbackStr DetailCallbackStr);
};

UCLASS(MinimalAPI, NotBlueprintType, NotBlueprintable)
class UDetailItemNumber :public UDetailItem
{
	GENERATED_BODY()
public:
	float NumberValue;
	FDetailCallbackNumber DetailCallbackNumber;
	UFUNCTION(BlueprintPure)
		static UDetailItem* GenDetailItemNumber(FName ItemTitle, float NumberValue, FDetailCallbackNumber DetailCallbackNumber);
};

UCLASS(MinimalAPI, NotBlueprintType, NotBlueprintable)
class UDetailItemCustom :public UDetailItem
{
	GENERATED_BODY()
public:
	class UUserWidget* CustomWidget;
	FDetailCallbackCustom DetailCallbackCustom;
	UFUNCTION(BlueprintPure)
		static UDetailItem* GenDetailItemCustom(FName ItemTitle, class UUserWidget* CustomWidget, FDetailCallbackCustom DetailCallbackCustom);
};

USTRUCT(BlueprintType)
struct FDetailCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FName Title;
	UPROPERTY(BlueprintReadWrite)
		TArray<UDetailItem*> ItemList;
};