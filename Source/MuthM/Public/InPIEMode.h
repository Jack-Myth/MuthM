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

	bool Exiting=false;
	float LastTime = 0;
public:
	virtual void NativeOnGameEnded(EGameEndReason GameEndReason) override;
	
protected:
#if WITH_EDITOR
	UFUNCTION()
		void TickPIE(float CurrentTime, float Duration);
#endif
	virtual void BindDelegates() override;
	void OnBackPressed();
	void OnExitPIE();

	virtual void BeginPlay() override;

};
