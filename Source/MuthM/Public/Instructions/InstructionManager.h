// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Instruction.h"
#include "SubclassOf.h"
#include "InstructionManager.generated.h"

struct FInstructionRecord
{
	TSubclassOf<UInstruction> InstructionClass;
};

//Ref of Instruction,Hold by Blueprint
USTRUCT(BlueprintType)
struct FInstructionRef
{
	GENERATED_BODY()
	TSharedPtr<FInstructionRecord>* RecordPtr;
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
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	static TScriptInterface<IInstructionManager> Get();

	UFUNCTION(BlueprintCallable)
		virtual FInstructionRef RegisterInstruction(const FName& InstructionName, TSubclassOf<UInstruction> InstructionClass) const = 0;

	UFUNCTION(BlueprintCallable)
		virtual bool UnregisterInstruction(const FInstructionRef InstructionRef) const = 0;

};
