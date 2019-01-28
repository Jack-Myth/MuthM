// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UMGBase/DetailInputCustomBase.h"
#include "DetailInputBoolBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UDetailInputBoolBase : public UDetailInputCustomBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		bool InputValue=false;
};
