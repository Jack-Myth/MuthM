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
	float Width=0.2;
	float SceneHalfWidth;
	class UParticleSystem* TapEffect=nullptr;
	float CheckWidthScale=1.f;
	float MaxScore = 100;
	UFUNCTION()
		void OnNumberPropertyChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
public:
	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args) override;

	virtual void OnPrepare_Implementation() override;
	virtual void OnTick_Implementation(float CurrentTime) override;
	UFUNCTION(BlueprintNativeEvent)
		void GetRhythmMaterial(class UMaterialInstanceDynamic*& ReturnedMaterial);
	UFUNCTION(BlueprintNativeEvent)
		void SetAlpha(class UMaterialInstanceDynamic* RhythmDMI,float Alpha);

	virtual bool OnBeginTouched_Implementation(float X, float Y) override;
	virtual float RequestPlainMaxScore_Implementation() override;
	virtual FBlueprintJsonObject GenArgsJsonObject_Implementation() override;
	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;
};
