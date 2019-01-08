// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Instruction.h"
#include "ScoreInfo.h"
#include "Rhythm.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class MUTHM_API URhythm : public UInstruction,public IScoreInfo
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly)
		float DefaultCheckWidthPerfect = 0.2f;
	UPROPERTY(BlueprintReadOnly)
		float DefaultCheckWidthGreat = 0.3f;
	UPROPERTY(BlueprintReadOnly)
		float DefaultCheckWidthSafe = 0.4f;
	UPROPERTY(BlueprintReadOnly)
		float DefaultCheckWidthBad = 0.5f;
public:

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		bool OnBeginTouched(float X,float Y);
};
