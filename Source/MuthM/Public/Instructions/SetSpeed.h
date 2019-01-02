// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Instruction.h"
#include "SetSpeed.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API USetSpeed : public UInstruction
{
	GENERATED_BODY()

	float Speed;
public:
	void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args);
	void OnPrepare_Implementation();
};
