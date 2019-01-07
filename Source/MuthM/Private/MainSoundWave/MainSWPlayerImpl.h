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

	TArray<FOnPlaybackPercent> OnPlaybackPercentDelegates;

	void OnPlaybackPercentCallback(const class UAudioComponent* AudioComponent,const class USoundWave* SoundWave,float PlaybackPercent);

protected:
	float _CachedCurPos = 0;
public:
	UMainSWPlayerImpl();
	
	virtual void SetMainSoundWave(TScriptInterface<class IMainSoundWave> MainSoundWave) override;


	virtual void Play(float StartTime = 0) override;


	virtual void SetPaused(bool Paused) override;


	virtual bool IsPlaying() const override;


	virtual void SetPitch(float newPitch = 1.f) override;


	virtual TScriptInterface<class IMainSoundWave> GetMainSoundWave() const override;


	virtual void Stop() override;


	virtual float GetPlaybackPosition() const override;


	virtual void AddOnPlaybackPercent(FOnPlaybackPercent Delegate) override;

};