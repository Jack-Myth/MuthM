// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MuthMGameModeBase.h"
#include "MMScript.h"
#include "MuthMInGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API AMuthMInGameMode : public AMuthMGameModeBase
{
	GENERATED_BODY()

	TArray<TScriptInterface<IMMScript>> MainMMSInstance;
	class FMDATFile* _pMDAT;
	TScriptInterface<IMMScript> _MainMMSInstance;
public:
	//GetScoreCore
	FORCEINLINE class FMDATFile* GetMDAT()
	{
		return _pMDAT;
	}
protected:
	virtual void BeginPlay() override;
};
