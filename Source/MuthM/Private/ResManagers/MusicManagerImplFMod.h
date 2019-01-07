// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MusicManagerImpl.h"
#include "MusicManagerImplFMod.generated.h"

/**
 * Implemention of IMusicManager
 */
UCLASS()
class UMusicManagerImplFMod : public UMusicManagerImpl
{
	GENERATED_BODY()
		//
public:
	virtual TScriptInterface<class IMainSoundWave> LoadMainSoundByID(int ID) override;

};
