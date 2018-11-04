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
public:
	virtual FInstructionRef RegisterInstruction(const FName& InstructionName, TSubclassOf<UInstruction> InstructionClass) const override;
	virtual bool UnregisterInstruction(const FInstructionRef InstructionRef) const override;

};
