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
	
	static TScriptInterface<IInstructionManager> Get();

	UFUNCTION(BlueprintCallable)
		virtual bool RegisterInstruction(const FName& InstructionName, const TSubclassOf<UInstruction>& InstructionClass, FInstructionRef& InstructionRef) = 0;

	UFUNCTION(BlueprintCallable)
		virtual void UnregisterInstruction(const FInstructionRef InstructionRef) = 0;

	virtual UInstruction* GenInstruction(FName InstructionName, FJsonObject& JsonArg) = 0;
	virtual void Tick(float CurrentTime) = 0;
	virtual void SetWorldContextProvider(UObject* Provider) = 0;
};