// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Instruction.h"
#include "ScoreInfo.h"
#include "Rhythm.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class MUTHM_API URhythm : public UInstruction,public IScoreInfo
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly)
		float DefaultCheckWidthPerfect = 0.2f;
	UPROPERTY(BlueprintReadOnly)
		float DefaultCheckWidthGreat = 0.3f;
	UPROPERTY(BlueprintReadOnly)
		float DefaultCheckWidthSafe = 0.4f;
	UPROPERTY(BlueprintReadOnly)
		float DefaultCheckWidthBad = 0.5f;

	UPROPERTY(BlueprintReadWrite)
		float CheckWidthScale = 1.f;

	UPROPERTY(BlueprintReadWrite)
		float SceneHalfWidth;
	UPROPERTY(BlueprintReadWrite)
		float Speed;

	UPROPERTY(BlueprintReadWrite)
		float LROffset;

	UFUNCTION()
		void OnRhythmNumberDetailsChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
public:

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		bool OnBeginTouched(float X,float Y);

	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args) override;
	virtual void OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EditorExtraInfo) override;
	
	virtual void OnPrepare_Implementation() override;
	virtual bool IsInstructionReady_Implementation() const override;

	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;
};
