// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InGameMode.h"
#include "InPIEMode.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API AInPIEMode : public AInGameMode
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnGameEnded(EGameEndReason GameEndReason) override;
	
protected:
	virtual void BindDelegates() override;
	void OnBackPressed();
};
