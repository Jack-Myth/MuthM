// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InstructionManager.h"
#include "InstructionManagerImpl.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable)
class MUTHM_API UInstructionManagerImpl : public UObject,public IInstructionManager
{
	GENERATED_BODY()

	UPROPERTY()
		TMap<FName, TSubclassOf<UInstruction>> _InstructionMap;
	UPROPERTY()
		TSet<UInstruction*> _InstructionInstanceList;
public:	
	virtual bool RegisterInstruction(const FName& InstructionName, const TSubclassOf<UInstruction>& InstructionClass, FInstructionRef& InstructionRef) override;
	virtual void UnregisterInstruction(const FInstructionRef InstructionRef) override;
	virtual bool ActiveInstruction(FName InstructionName, FJsonObject& JsonArg) override;

	//Called by the other function ,so DestroyReason must be provided.
	virtual bool DestroyInstructionInstance(UInstruction* InstructionInstance, EInstructionDestroyReason DestroyReason) override;
	
};
