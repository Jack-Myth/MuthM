// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainSWPlayer.h"
#include "MainSWPlayerImpl.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UMainSWPlayerImpl : public UObject, public IMainSWPlayer
{
	GENERATED_BODY()

	UPROPERTY()
		class UAudioComponent* audioComponent;
public:
	UMainSWPlayerImpl();
	
	virtual void SetMainSoundWave(TScriptInterface<class IMainSoundWave> MainSoundWave) override;


	virtual void Play(float StartTime) override;


	virtual void SetPaused(bool Paused) override;


	virtual bool IsPlaying() const override;


	virtual void SetPlaySpeed(float newSpeed = 1.f) override;

};