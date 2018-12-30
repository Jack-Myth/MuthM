// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "MainSoundWave.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UMainSoundWave : public USoundWave
{
	friend class UMuthMBPLib;
	GENERATED_BODY()
public:
	//This function may freeze the engine
	bool DecodePCMFromCompressedData(TArray<uint8>& OutputPCM);
	FORCEINLINE int32 GetSampleRate()
	{
		return SampleRate;
	}
};
