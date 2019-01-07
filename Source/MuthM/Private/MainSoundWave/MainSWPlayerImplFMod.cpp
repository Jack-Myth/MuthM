// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainSWPlayerImplFMod.h"
#include "MainSoundWaveImplFMod.h"
#include "fmod.hpp"
#include "MuthMNativeLib.h"
#include "Engine/World.h"
#include "TimerManager.h"

#ifdef _MUTHM_USE_FMOD

void UMainSWPlayerImplFMod::OnTick()
{
	if (!IsPlaying())
		return;
	uint32 Position,Duration;
	pFModChannel->getPosition(&Position, FMOD_TIMEUNIT_MS);
	pMainSoundWave->GetFModSound().Get()->getLength(&Duration,FMOD_TIMEUNIT_MS);
	float CurPos = (float)Position / Duration;
	for (auto it=OnPlaybackPercentDelegates.CreateIterator();it;++it)
	{
		if (it->IsBound())
			it->Execute(pMainSoundWave, CurPos);
		else
			it.RemoveCurrent();
	}
}

void UMainSWPlayerImplFMod::SetMainSoundWave(TScriptInterface<class IMainSoundWave> MainSoundWave)
{
	Stop();
	pMainSoundWave = Cast<UMainSoundWaveImplFMod>(MainSoundWave.GetObject());
	FMOD::Channel* pTmpFModChannel;
	if (MuthMNativeLib::GetFModSystem()->playSound(pMainSoundWave->GetFModSound().Get(), nullptr, true, &pTmpFModChannel) == FMOD_OK)
	{
		pFModChannel = MakeShareable(pTmpFModChannel);
		if (!TickHandle.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(TickHandle, this, &UMainSWPlayerImplFMod::OnTick, 0, true, 0);
		}
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
		pFModChannel->isPlaying(&bIsPlaying);
		return bIsPlaying;
	}
	return false;
}

void UMainSWPlayerImplFMod::SetPitch(float newPitch /*= 1.f*/)
{
	if (pFModChannel)
	{
		pFModChannel->setPitch(newPitch);
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
}

float UMainSWPlayerImplFMod::GetPlaybackPosition() const
{
	if (pFModChannel)
	{
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

#endif