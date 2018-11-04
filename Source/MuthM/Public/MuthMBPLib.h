// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InstructionManager.h"
#include "MuthMBPLib.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UMuthMBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
		static TScriptInterface<IInstructionManager> GetInstructionManager();
};
