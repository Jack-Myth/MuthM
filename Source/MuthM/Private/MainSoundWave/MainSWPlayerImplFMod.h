// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainSWPlayer.h"
#include "MainSWPlayerImplFMod.generated.h"

/**
 *
 */
UCLASS()
class MUTHM_API UMainSWPlayerImplFMod : public UObject, public IMainSWPlayer
{
	GENERATED_BODY()

	UProperty()
		class UMainSoundWaveImplFMod* pMainSoundWave=nullptr;
	class FMOD::Channel* pFModChannel=nullptr;
public:
	UMainSWPlayerImpl();

	virtual void SetMainSoundWave(TScriptInterface<class IMainSoundWave> MainSoundWave) override;


	virtual void Play(float StartTime) override;


	virtual void SetPaused(bool Paused) override;


	virtual void Stop() override;


	virtual bool IsPlaying() const override;


	virtual void SetPitch(float newPitch = 1.f) override;

};