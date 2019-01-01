// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Rhythm.h"
#include "NoExportTypes.h"
#include "RhythmTap.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API URhythmTap : public URhythm
{
	GENERATED_BODY()

protected:
	float LROffset;
	class AStaticMeshActor* RhythmObj;
	FLinearColor RhythmColor;
public:
	void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args);

	void OnPrepare_Implementation();
	void OnTick_Implementation(float CurrentTime);
	UFUNCTION(BlueprintNativeEvent)
		void GetRhythmMaterial(class UMaterialInterface*& ReturnedMaterial);
};
