// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InGameMode.h"
#include "InPIEMode.generated.h"

DECLARE_DELEGATE(FOnExitPIE);

/**
 * 
 */
UCLASS()
class MUTHM_API AInPIEMode : public AInGameMode
{
	GENERATED_BODY()
	
public:
	FOnExitPIE OnExitPIEDelegate;
	virtual void NativeOnGameEnded(EGameEndReason GameEndReason) override;

};
