// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Instruction.h"
#include "GlobalVariable.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UGlobalVariable : public UInstruction
{
	GENERATED_BODY()
	
public:
	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;
	FBlueprintJsonObject GenArgsJsonObject_Implementation() override;

};
