// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Instruction.h"
#include "LoadScene.generated.h"

/**
 * 
 */
UCLASS()
class ULoadScene : public UInstruction
{
	GENERATED_BODY()
public:
	void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args);
};
