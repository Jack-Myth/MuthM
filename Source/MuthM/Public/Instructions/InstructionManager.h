// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Instruction.h"
#include "SubclassOf.h"
#include "JsonObject.h"
#include "MMScript.h"
#include "InstructionManager.generated.h"

//Ref of Instruction,Hold by Blueprint7
USTRUCT(BlueprintType)
struct FInstructionRef
{
	GENERATED_BODY()
	FName InstructionName;
	TSubclassOf<UInstruction> RecordPtr;
};

USTRUCT()
struct FInstructionStack
{
	GENERATED_BODY()
	TArray<TSubclassOf<UInstruction>> ClassStack;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UInstructionManager : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MUTHM_API IInstructionManager
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	static TScriptInterface<IInstructionManager> Get(const UObject* WorldContextObj);

	UFUNCTION(BlueprintCallable)
		virtual bool RegisterInstruction(const FName& InstructionName, const TSubclassOf<UInstruction>& InstructionClass, FInstructionRef& InstructionRef) = 0;

	UFUNCTION(BlueprintCallable)
		virtual void UnregisterInstruction(const FInstructionRef InstructionRef) = 0;
	UFUNCTION(BlueprintCallable)
		virtual FName GetInstructionName(FInstructionRef InstructionRef) const = 0;
	virtual FName GetInstructionName(TSubclassOf<UInstruction> InstructionClass) const = 0;
	virtual UInstruction* GenInstruction(FName InstructionName,float Time) = 0;
	virtual void Tick(float CurrentTime) = 0;
	virtual void DestroyMMScriptInstance(TScriptInterface<IMMScript> TargetMMSInstance) = 0;
	virtual TScriptInterface<IMMScript> GenMMScript(bool bIsEditorMode = false) = 0;
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GenMMScript"))
		virtual TScriptInterface<IMMScript> K2_GenMMScript() = 0;
	UFUNCTION(BlueprintCallable)
		virtual TArray<FName> ListAllInstructionName()const = 0;
};