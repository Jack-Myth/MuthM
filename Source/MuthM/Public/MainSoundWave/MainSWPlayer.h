// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainSWPlayer.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UMainSWPlayer : public UInterface
{
	GENERATED_BODY()

};

/**
 *
 */
class MUTHM_API IMainSWPlayer
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		static TScriptInterface<IMainSWPlayer> GenMainSWPlayer();

	UFUNCTION(BlueprintCallable)
		virtual void SetMainSoundWave(TScriptInterface<class IMainSoundWave> MainSoundWave) = 0;

	UFUNCTION(BlueprintCallable)
		virtual void Play(float StartTime) = 0;

	UFUNCTION(BlueprintCallable)
		virtual void SetPaused(bool Paused) = 0;

	UFUNCTION(BlueprintCallable)
		virtual void Stop() = 0;

	UFUNCTION(BlueprintCallable)
		virtual	bool IsPlaying() const = 0;

	UFUNCTION(BlueprintCallable)
		virtual void SetPitch(float newPitch = 1.f) = 0;
};
