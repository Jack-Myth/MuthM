// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainSoundWave.h"
#include "MainSoundWaveImplFMod.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UMainSoundWaveImplFMod : public UObject,public IMainSoundWave
{
	GENERATED_BODY()
	
	TSharedPtr<class FMOD::Sound> pFModSound;
public:

	virtual bool GenPCMData(TArray<uint8>& OutputPCM) override = 0;

	TSharedPtr<class FMOD::Sound> GetFModSound()
	{
		return pFModSound;
	}
};
