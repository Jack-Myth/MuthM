// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "MainSWPlayerImpl.h"
#include "MainSoundWave.h"
#include "Components/AudioComponent.h"

UMainSWPlayerImpl::UMainSWPlayerImpl()
{
	audioComponent = CreateDefaultSubobject<UAudioComponent>(NAME_None);
	audioComponent->bAutoDestroy = false;
}

void UMainSWPlayerImpl::SetMainSoundWave(TScriptInterface<IMainSoundWave> MainSoundWave)
{
	audioComponent->SetSound(MainSoundWave);
}

void UMainSWPlayerImpl::Play(float StartTime)
{
	audioComponent->Play(StartTime);
}

void UMainSWPlayerImpl::SetPaused(bool Paused)
{
	audioComponent->SetPaused(Paused)
}

bool UMainSWPlayerImpl::IsPlaying() const
{
	return audioComponent->IsPlaying();
}

void UMainSWPlayerImpl::SetPlaySpeed(float newSpeed /*= 1.f*/)
{
	audioComponent->SetPitchMultiplier(newSpeed);
}

