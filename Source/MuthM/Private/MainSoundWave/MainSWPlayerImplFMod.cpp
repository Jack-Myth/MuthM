// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "MainSWPlayerImplFMod.h"
#include "MainSoundWaveImplFMod.h"
#include "fmod.hpp"
#include "FMODStudioModule.h"
#include "MuthMNativeLib.h"

#ifdef _MUTHM_USE_FMOD
void UMainSWPlayerImplFMod::SetMainSoundWave(TScriptInterface<class IMainSoundWave> MainSoundWave)
{
	Stop();
	pMainSoundWave = Cast<UMainSoundWaveImplFMod>(MainSoundWave.GetObject());
	MuthMNativeLib::GetFModSystem()->playSound(pMainSoundWave->GetFModSound().Get(),nullptr,true,&pFModChannel);
}

void UMainSWPlayerImplFMod::Stop()
{
	auto pFModSound = pMainSoundWave->GetFModSound();
	IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime)->
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

#endif