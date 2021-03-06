// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainSoundWave.h"
#include "MainSoundWaveImplFMod.generated.h"

namespace FMOD
{ 
	class Sound;
}

/**
 * 
 */
UCLASS()
class MUTHM_API UMainSoundWaveImplFMod : public UObject,public IMainSoundWave
{
	GENERATED_BODY()
	
	class FMOD::Sound* pFModSound=nullptr;
protected:
	void ReleaseSound();
public:

	virtual bool GenPCMData(TArray<uint8>& OutputPCM) override;

	class FMOD::Sound* GetFModSound()
	{
		return pFModSound;
	}

	virtual float GetSoundDuration() const override;


	virtual int32 GetSampleRate() const override;


	virtual int32 GetNumChannels() const override;


	void UpdateSoundResource(class FMOD::Sound* newFModSound);


	virtual ~UMainSoundWaveImplFMod();
};
