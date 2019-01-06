// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainSoundWave.h"
#include "Sound/SoundWave.h"
#include "MainSoundWaveImpl.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UMainSoundWaveImpl : public USoundWave,public IMainSoundWave
{
	GENERATED_BODY()
	
public:

	virtual bool GenPCMData(TArray<uint8>& OutputPCM) override = 0;
};
