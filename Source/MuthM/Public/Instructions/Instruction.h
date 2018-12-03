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
	TSharedPtr<UWorld> CachedWorld;

	UPROPERTY(BlueprintGetter="GetScript")
		TScriptInterface<class IMMScript> AttachedScript = nullptr;
public:
	void SetWorld(UWorld* WorldContext);
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
		float GetTime();
	UFUNCTION(BlueprintSetter)
		void SetTime(float pNewTime);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionLoaded(FBlueprintJsonObject Args);
	UFUNCTION(BlueprintImplementableEvent)
		void OnPrepare();
	UFUNCTION(BlueprintImplementableEvent)
		void OnTick(float CurrentTime);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionDestroy(EInstructionDestroyReason DestroyReason);
	UFUNCTION(BlueprintImplementableEvent)
		void OnTimeArrived();
	UFUNCTION(BlueprintImplementableEvent)
		FBlueprintJsonObject GenArgsJsonObject();
	UFUNCTION(Blueprintcallable)
		void DestroySelf();
	UFUNCTION(BlueprintNativeEvent)
		class UInstructionWidgetBase* GenInstructionWidget();

	//Override GetWorld() To provide the WorldContext
	virtual class UWorld* GetWorld() const override final
	{
		return CachedWorld.Get();
	}

};