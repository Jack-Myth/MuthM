// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interface.h"
#include "Instruction.h"
#include "SubclassOf.h"
#include "MMScript.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MMScript, Log, All)

UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UMMScript : public UInterface
{
	GENERATED_BODY()

};

class MUTHM_API IMMScript
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		virtual bool LoadFromFile(FString FileName) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool LoadFromData(const TArray<uint8>& FileData) = 0;
	//virtual void AddInstruction() = 0;
	virtual	void RemoveInstructionByType(TSubclassOf<UInstruction> InstructionType,EInstructionDestroyReason Reason) = 0;
	virtual void RemoveInstruction(UInstruction* Instance, EInstructionDestroyReason Reason) = 0;
	UFUNCTION(BlueprintCallable)
		virtual float GetRemainingInstructionCount() = 0;
};

