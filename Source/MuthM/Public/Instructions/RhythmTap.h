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
	UPROPERTY(BlueprintReadWrite)
		class AStaticMeshActor* RhythmObj;
	UPROPERTY(BlueprintReadWrite)
		FLinearColor RhythmColor;
	UPROPERTY(BlueprintReadWrite)
		UMaterialInstanceDynamic* RhythmTapMatDynamic;
	UPROPERTY(BlueprintReadWrite)
		class UParticleSystem* TapEffect=nullptr;
	UPROPERTY(BlueprintReadWrite)
		float MaxScore = 100;
	UFUNCTION()
		void OnNumberPropertyChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
	UFUNCTION(BlueprintCallable)
		void InitProperty(UPARAM(Ref) FBlueprintJsonObject& Args);
public:
	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args) override;
	virtual void OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args,FEditorExtraInfo EditorExtraInfo) override;
	virtual void OnPrepare_Implementation() override;
	virtual void OnTick_Implementation(float CurrentTime) override;
	UFUNCTION(BlueprintNativeEvent)
		void GetRhythmMaterial(class UMaterialInstanceDynamic*& ReturnedMaterial);
	UFUNCTION(BlueprintNativeEvent)
		void SetAlpha(class UMaterialInstanceDynamic* RhythmDMI,float Alpha);

	virtual ERhythmTouchResult OnTouchBegin_Implementation(float X, float YPercent) override;
	virtual float RequestPlainMaxScore_Implementation() override;
	virtual FBlueprintJsonObject GenArgsJsonObject_Implementation() override;
	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;
};
