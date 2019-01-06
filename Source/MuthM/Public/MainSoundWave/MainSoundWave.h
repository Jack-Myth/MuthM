// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MainSoundWave.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UMainSoundWave : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MUTHM_API IMainSoundWave
{
	GENERATED_BODY()
public:
	virtual bool GenPCMData(TArray<uint8>& OutputPCM) = 0;

};
