// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ScoreInfo.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UScoreInfo : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MUTHM_API IScoreInfo
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
		float RequestPlainMaxScore();

};
