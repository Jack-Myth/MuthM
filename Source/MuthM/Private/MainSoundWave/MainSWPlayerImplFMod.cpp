// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainSWPlayerImplFMod.h"
#include "MainSoundWaveImplFMod.h"
#include "fmod.hpp"
#include "MuthMNativeLib.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

#ifdef _MUTHM_USE_FMOD

void UMainSWPlayerImplFMod::SetMainSoundWave(TScriptInterface<class IMainSoundWave> MainSoundWave)
{
	Stop();
	pMainSoundWave = Cast<UMainSoundWaveImplFMod>(MainSoundWave.GetObject());
	FMOD::Channel* pTmpFModChannel;
	if (!pMainSoundWave->GetFModSound())
		return;
	if (MuthMNativeLib::GetFModSystem()->playSound(pMainSoundWave->GetFModSound(), nullptr, true, &pTmpFModChannel) == FMOD_OK)
	{
		if (!pSpectrumDSP)
		{
			MuthMNativeLib::GetFModSystem()->createDSPByType(FMOD_DSP_TYPE_FFT, &pSpectrumDSP);
		}
		else if (pFModChannel)
		{
			pFModChannel->removeDSP(pSpectrumDSP);
		}
		pFModChannel = pTmpFModChannel;
		pFModChannel->addDSP(0, pSpectrumDSP);
	}
}

void UMainSWPlayerImplFMod::Stop()
{
	if (!pMainSoundWave)
		return;
	auto pFModSound = pMainSoundWave->GetFModSound();
	pFModChannel->stop();
}

void UMainSWPlayerImplFMod::Play(float StartTime)
{
	if (pFModChannel)
	{
		pFModChannel->setPosition(StartTime * 1000, FMOD_TIMEUNIT_MS);
		pFModChannel->setPaused(false);
	}
}

void UMainSWPlayerImplFMod::SetPaused(bool Paused)
{
	if (pFModChannel)
	{
		pFModChannel->setPaused(Paused);
	}
}

bool UMainSWPlayerImplFMod::IsPlaying() const
{
	if (pFModChannel)
	{
		bool bIsPlaying;
		pFModChannel->getPaused(&bIsPlaying);
		return !bIsPlaying;
	}
	return false;
}

void UMainSWPlayerImplFMod::SetPitch(float newPitch /*= 1.f*/)
{
	if (pFModChannel)
	{
		pFModChannel->setPitch(newPitch);
		Pitch = newPitch;
	}
}

TScriptInterface<class IMainSoundWave> UMainSWPlayerImplFMod::GetMainSoundWave() const
{
	return pMainSoundWave;
}

UMainSWPlayerImplFMod::~UMainSWPlayerImplFMod()
{
	if (IsPlaying())
		Stop();
	if (pSpectrumDSP)
		pSpectrumDSP->release();
}

float UMainSWPlayerImplFMod::GetPlaybackPosition() const
{
	if (pFModChannel)
	{
		MuthMNativeLib::GetFModSystem()->update();
		uint32 Position;
		pFModChannel->getPosition(&Position, FMOD_TIMEUNIT_MS);
		return Position / 1000.f;
	}
	return 0;
}

void UMainSWPlayerImplFMod::AddOnPlaybackPercent(FOnPlaybackPercent Delegate)
{
	OnPlaybackPercentDelegates.Add(Delegate);
}

void UMainSWPlayerImplFMod::Tick(float DeltaTime)
{
	if (!IsPlaying())
		return;
	uint32 Position, Duration;
	pFModChannel->getPosition(&Position, FMOD_TIMEUNIT_MS);
	PositionOffsetMs += DeltaTime * Pitch * 1000;
	if (bPlaybackEventInterpolation)
	{
		if (Position!=LastTimeMs)
		{
			if (LastTimeMs + PositionOffsetMs<Position||
				(int)(LastTimeMs + PositionOffsetMs) - (int)Position > Pitch * 1000)
			{
				//Fix wrong offset(larger than 1 second)
				PositionOffsetMs = 0;
				LastTimeMs = Position;
			}
			else
			{
				//No more than a tick.
				PositionOffsetMs = LastTimeMs + PositionOffsetMs - Position;
				LastTimeMs = Position;
				Position = LastTimeMs + PositionOffsetMs;
			}
		}
		else
		{
			Position += PositionOffsetMs;
		}
	}
	pMainSoundWave->GetFModSound()->getLength(&Duration, FMOD_TIMEUNIT_MS);
	float CurPos = (float)Position / Duration;
	for (auto it = OnPlaybackPercentDelegates.CreateIterator(); it; ++it)
	{
		if (it->IsBound())
			it->Execute(pMainSoundWave, CurPos);
		else
			it.RemoveCurrent();
	}
}

TStatId UMainSWPlayerImplFMod::GetStatId() const
{
	return TStatId();
}

void UMainSWPlayerImplFMod::SetPlaybackEventInterpolation(bool ShouldEnable)
{
	bPlaybackEventInterpolation = ShouldEnable;
}

bool UMainSWPlayerImplFMod::GetPlaybackEventInterpolation() const
{
	return bPlaybackEventInterpolation;
}

void UMainSWPlayerImplFMod::GetSepctrum(bool bSplitChannel, int32 Width, TArray<float>& SpectrumL, TArray<float>& SpectrumR) const
{
	if (!pSpectrumDSP)
		return;
	FMOD_DSP_PARAMETER_FFT* pData;
	unsigned int length;
	pSpectrumDSP->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void**)&pData, &length, nullptr, 0);
	if (!pData)
		return;
	int32 FFTDataLength = pData->length / 2.f;
	//LChannel
	for (int i = 0; i < FFTDataLength; i++)
		SpectrumL.Add(pData->spectrum[0][i]);
	//RChannel
	if (pData->numchannels>=2) //Check if have 2 channels
		for (int i = 0; i < FFTDataLength; i++)
			SpectrumR.Add(pData->spectrum[1][i]);
	if (!bSplitChannel)
	{
		for (int i = 0; i < FFTDataLength; i++)
			SpectrumL[i] = (SpectrumL[i] + SpectrumR[i]) / 2.f;
		SpectrumR.Empty();
	}
	//Scale Width and convert to DB
	if (Width>0&&Width<FFTDataLength)
	{
		int CurIndex = 0;
		int ScaledIndex = 0;
		int SplitBlock = FFTDataLength / Width;
		int ModRes = FFTDataLength % Width;
		for (CurIndex=0;CurIndex<FFTDataLength;CurIndex+=SplitBlock+((ModRes--)>0),ScaledIndex++)
		{
			float tmpSpectrumSum=0;
			for (int i=CurIndex;i< CurIndex+SplitBlock +(ModRes>0);i++)
				tmpSpectrumSum += SpectrumL[i];
			SpectrumL[ScaledIndex] = tmpSpectrumSum / (SplitBlock + (ModRes > 0));
			if (bSplitChannel)
			{
				tmpSpectrumSum = 0;
				for (int i = CurIndex; i < CurIndex + SplitBlock + (ModRes > 0); i++)
					tmpSpectrumSum += SpectrumL[i];
				SpectrumR[ScaledIndex] = tmpSpectrumSum / (SplitBlock + (ModRes > 0));
			}
		}
		SpectrumL.SetNum(Width);
		if (bSplitChannel)
			SpectrumR.SetNum(Width);
	}
}

#endif