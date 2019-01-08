// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Classes/BlueprintJsonLibrary.h"
#include "HasDetails.h"
#include "Instruction.generated.h"

UENUM(BlueprintType)
enum class EInstructionDestroyReason :uint8
{
	IDR_Manually UMETA(DisplayName = "Manually"),
	IDR_Unregister UMETA(DisplayName = "Unregister"),
	IDR_CleanScene UMETA(DisplayName = "CleanScene"),
	IDR_ScriptDeleted UMETA(DisplayName = "ScriptDeleted"),
	IDR_Editing	UMETA(DisplayName="Editing"),
	IDR_ExitPIE UMETA(DisplayName = "ExitPIE")
};

/**
 * The Base UClass of all MMS Instructions
 */
UCLASS(BlueprintType,Blueprintable,Abstract)
class MUTHM_API UInstruction:public UObject,public IHasDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintGetter="GetTime",BlueprintSetter="SetTime")
		float InstructionTime;

	UPROPERTY(BlueprintGetter="GetScript")
		TScriptInterface<class IMMScript> AttachedScript = nullptr;
	bool bIsFinished = false;
protected:
	UFUNCTION()
		void OnTimeChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
public:

	void AttachScript(TScriptInterface<class IMMScript> TargetScript)
	{
		AttachedScript = TargetScript;
	}
	UFUNCTION(BlueprintGetter)
		TScriptInterface<class IMMScript> GetScript()
	{
		return AttachedScript;
	}
	UFUNCTION(BlueprintGetter)
		FORCEINLINE float GetTime() const
	{
		return InstructionTime;
	}
	UFUNCTION(BlueprintSetter)
		void SetTime(float NewTime)
	{
		InstructionTime = NewTime;
	}
	UFUNCTION(BlueprintNativeEvent)
		void OnInstructionLoaded(FBlueprintJsonObject Args);
	UFUNCTION(BlueprintNativeEvent)
		void OnPrepare();
	UFUNCTION(BlueprintNativeEvent)
		void OnTick(float CurrentTime);
	UFUNCTION(BlueprintNativeEvent)
		void OnInstructionDestroy(EInstructionDestroyReason DestroyReason);
	UFUNCTION(BlueprintNativeEvent)
		void OnTimeArrived();
	UFUNCTION(BlueprintNativeEvent)
		FBlueprintJsonObject GenArgsJsonObject();
	UFUNCTION(BlueprintCallable,meta=(ToolTip="Please Call this funtion after instruction is finished."))
		void DestroySelf();
	UFUNCTION(BlueprintNativeEvent)
		class UInstructionWidgetBase* GenInstructionWidget();
	UFUNCTION(BlueprintPure)
		float GetGlobalNumberData(FName Key) const;
	UFUNCTION(BlueprintCallable)
		void SetGlobalNumberData(FName Key, float Value);
	//Override GetWorld() To provide the WorldContext
	virtual class UWorld* GetWorld() const override final
	{
		return GetOuter()->GetWorld();
	}

	void OnBuildingDetails_Implementation(const TScriptInterface<IDetailsBuilder>& DetailsBuilder);
};