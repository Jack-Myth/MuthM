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
	float Speed;
	UMaterialInstanceDynamic* RhythmTapMatDynamic;
	float Width;
	float SceneHalfWidth;
	class UParticleSystem* TapEffect=nullptr;
	float CheckWidthScale=1.f;
	float MaxScore = 100;
public:
	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args);

	virtual void OnPrepare_Implementation();
	virtual void OnTick_Implementation(float CurrentTime);
	UFUNCTION(BlueprintNativeEvent)
		void GetRhythmMaterial(class UMaterialInstanceDynamic*& ReturnedMaterial);
	UFUNCTION(BlueprintNativeEvent)
		void SetAlpha(class UMaterialInstanceDynamic* RhythmDMI,float Alpha);

	virtual bool OnBeginTouched_Implementation(float X, float Y);
	virtual float RequestPlainMaxScore_Implementation();
};
