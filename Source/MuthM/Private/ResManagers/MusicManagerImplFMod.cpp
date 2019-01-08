// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MusicManagerImplFMod.h"
#include "fmod.hpp"
#include "FileManager.h"
#include "MuthMNativeLib.h"
#include "MainSoundWave/MainSoundWaveImplFMod.h"

#ifdef _MUTHM_USE_FMOD

TScriptInterface<class IMainSoundWave> UMusicManagerImplFMod::LoadMainSoundByID(int ID)
{
	FString TargetFileName;
	if (ID < 0)
		TargetFileName = UMusicManagerImpl::ConstructOfflineMusicFileName(ID);
	else
		TargetFileName = UMusicManagerImpl::ConstructMusicFileName(ID);
	if (IFileManager::Get().FileExists(*TargetFileName))
	{
		FMOD_CREATESOUNDEXINFO exinfo = { NULL };
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_OGGVORBIS;
		class FMOD::Sound* pSound = nullptr;
		MuthMNativeLib::GetFModSystem()->createSound(TCHAR_TO_UTF8(*TargetFileName), FMOD_DEFAULT, &exinfo, &pSound);
		if (pSound)
		{
			UMainSoundWaveImplFMod* MainSoundWave = NewObject<UMainSoundWaveImplFMod>(this);
			MainSoundWave->UpdateSoundResource(pSound);
			return MainSoundWave;
		}
	}
	return NewObject<UMainSoundWaveImplFMod>(this);
}

#endif // _MUTHM_USE_FMOD