// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainSWPlayer.h"
#include "Tickable.h"
#include "MainSWPlayerImplFMod.generated.h"

namespace FMOD
{
	class Channel;
	class DSP;
}

/**
 *
 */
UCLASS()
class MUTHM_API UMainSWPlayerImplFMod : public UObject, public IMainSWPlayer,public FTickableGameObject
{
	GENERATED_BODY()

	UPROPERTY()
		class UMainSoundWaveImplFMod* pMainSoundWave=nullptr;
	class FMOD::Channel* pFModChannel;
	TArray<FOnPlaybackPercent> OnPlaybackPercentDelegates;

private:
	uint32 PositionOffsetMs = 0;

	uint32 LastTimeMs = 0;

	bool bPlaybackEventInterpolation = true;
	float Pitch = 1.f;
	bool bGenSpectrum = true;
	class FMOD::DSP* pSpectrumDSP = nullptr;
public:

	virtual void SetMainSoundWave(TScriptInterface<class IMainSoundWave> MainSoundWave) override;


	virtual void Play(float StartTime = 0) override;


	virtual void SetPaused(bool Paused) override;


	virtual void Stop() override;


	virtual bool IsPlaying() const override;


	virtual void SetPitch(float newPitch = 1.f) override;


	virtual TScriptInterface<class IMainSoundWave> GetMainSoundWave() const override;


	virtual ~UMainSWPlayerImplFMod();

	virtual float GetPlaybackPosition() const override;



	virtual void AddOnPlaybackPercent(FOnPlaybackPercent Delegate) override;


	virtual void Tick(float DeltaTime) override;


	virtual TStatId GetStatId() const override;


	virtual void SetPlaybackEventInterpolation(bool ShouldEnable) override;


	virtual bool GetPlaybackEventInterpolation() const override;


	virtual void GetSepctrum(bool bSplitChannel, int32 Width, TArray<float>& SpectrumL, TArray<float>& SpectrumR) const override;

};