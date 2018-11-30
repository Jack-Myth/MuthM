// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "VisualizableSoundWave.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UVisualizableSoundWave : public USoundWave
{
	friend UMuthMBPLib;
	GENERATED_BODY()

	UPROPERTY()
		TArray<uint8> _CachedStdPCM;
public:
	UFUNCTION(BlueprintCallable)
	inline bool GetCachedPCM(TArray<uint8>& OuptutPCM)
	{
		if (_CachedStdPCM.Num())
		{
			OuptutPCM = _CachedStdPCM;
			return true;
		}
		return false;
	}

	//If SplitChannels is true,OutSpectrums will have two arrays,the first is L Channel.
	//Otherwise OutSpectrums will contain only one array,is the mixed Channel.
	void CalculateFrequencySpectrum(bool SplitChannels, const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<TArray<float>>& OutSpectrums);
	UFUNCTION(BlueprintCallable)
		void CalculateFrequencySpectrumSplit(const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<float>& LSpectrum, TArray<float>& RSpectrum);
	UFUNCTION(BlueprintCallable)
		void CalculateFrequencySpectrumMixed(const float BeginTime, const float SampleTimeLength, const int32 SpectrumWidth, TArray<float>& MixedSpectrum);

	//Note:It may need a lot time to do the job,don't use it for real time.
	UFUNCTION(BlueprintCallable)
		float CalculateBPM();
};
