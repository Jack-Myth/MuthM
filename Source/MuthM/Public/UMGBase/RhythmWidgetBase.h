// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UMGBase/InstructionWidgetBase.h"
#include "RhythmWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API URhythmWidgetBase : public UInstructionWidgetBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite)
		float WidthPercent = 0.2f;
public:

	UFUNCTION(BlueprintCallable)
		void SetWidthPercent(float newWidthPercent);
};
