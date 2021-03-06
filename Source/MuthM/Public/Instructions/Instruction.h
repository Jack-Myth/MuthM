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

USTRUCT(BlueprintType)
struct FEditorExtraInfo
{
	GENERATED_BODY()
		//
	bool ExtraInfoValid = false;
	float VerticalOffset=0;
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
	//bool bIsFinished = false;
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		bool bHasPriority = false;
	UFUNCTION()
		void OnTimeChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
public:

	float EditorVisualVerticalOffset;

	UFUNCTION(BlueprintPure,meta=(ToolTip="Return if this instruction have high priority"))
		FORCEINLINE bool HasPriority() const
	{
		return bHasPriority;
	}

	UFUNCTION(BlueprintPure)
		FName GetRegisterName() const;

	void AttachScript(TScriptInterface<class IMMScript> TargetScript)
	{
		AttachedScript = TargetScript;
	}
	UFUNCTION(BlueprintGetter)
		TScriptInterface<class IMMScript> GetScript() const
	{
		return AttachedScript;
	}

	UFUNCTION(BlueprintNativeEvent)
		bool IsInstructionReady(float CurrentTime) const;
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
		void OnInstructionLoaded_Editor(FBlueprintJsonObject Args,FEditorExtraInfo EditorExtraInfo);
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
	//Override GetWorld() To provide the WorldContext
	virtual class UWorld* GetWorld() const override;

	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;

protected:
	//Global Data Operation.
	UFUNCTION(BlueprintPure)
		float GetGlobalNumberData(FName Key) const;
	UFUNCTION(BlueprintCallable)
		void SetGlobalNumberData(FName Key, float Value);
	UFUNCTION(BlueprintPure)
		FString GetGlobalStringData(FName Key) const;
	UFUNCTION(BlueprintCallable)
		void SetGlobalStringData(FName Key, FString String);
	UFUNCTION(BlueprintPure)
		UObject* GetGlobalObjectData(FName Key) const;
	UFUNCTION(BlueprintCallable)
		void SetGlobalObjectData(FName Key, UObject* Object);
};