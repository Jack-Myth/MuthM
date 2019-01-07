// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MainSWPlayerImpl.h"
#include "MainSoundWave.h"
#include "Components/AudioComponent.h"

void UMainSWPlayerImpl::OnPlaybackPercentCallback(const class UAudioComponent* AudioComponent, const class USoundWave* SoundWave, float PlaybackPercent)
{
	for (auto it=OnPlaybackPercentDelegates.CreateIterator();it;++it)
	{
		if (it->IsBound())
			it->Execute(GetMainSoundWave(), PlaybackPercent);
		else
			it.RemoveCurrent();
	}
}

UMainSWPlayerImpl::UMainSWPlayerImpl()
{
	audioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	audioComponent->bAutoDestroy = false;
	audioComponent->OnAudioPlaybackPercentNative.AddUObject(this, &UMainSWPlayerImpl::OnPlaybackPercentCallback);
}

void UMainSWPlayerImpl::SetMainSoundWave(TScriptInterface<IMainSoundWave> MainSoundWave)
{
	audioComponent->SetSound((USoundBase*)MainSoundWave.GetObject());
}

void UMainSWPlayerImpl::Play(float StartTime)
{
	audioComponent->Play(StartTime);
}

void UMainSWPlayerImpl::SetPaused(bool Paused)
{
	audioComponent->SetPaused(Paused);
}

bool UMainSWPlayerImpl::IsPlaying() const
{
	return audioComponent->IsPlaying();
}

void UMainSWPlayerImpl::SetPitch(float newPitch /*= 1.f*/)
{
	audioComponent->SetPitchMultiplier(newPitch);
}

TScriptInterface<class IMainSoundWave> UMainSWPlayerImpl::GetMainSoundWave() const
{
	return audioComponent->Sound;
}

void UMainSWPlayerImpl::Stop()
{
	audioComponent->Stop();
}

float UMainSWPlayerImpl::GetPlaybackPosition() const
{
	return _CachedCurPos;
}

void UMainSWPlayerImpl::AddOnPlaybackPercent(FOnPlaybackPercent Delegate)
{
	OnPlaybackPercentDelegates.Add(Delegate);
}
