// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Instruction.h"
#include "ScoreInfo.h"
#include "Rhythm.generated.h"

UENUM(BlueprintType)
enum class ERhythmTouchResult: uint8
{
	RTR_Continue UMETA(DisplayName = "Continue", ToolTip = "This Rhythm doesn't accept,try next one."),
	RTR_Accepted UMETA(DisplayName = "Accepted", ToolTip = "This Rhythm accepted,no need to check next."),
	RTR_Tracking UMETA(DisplayName = "Tracking", ToolTip = "This Rhythm need to tracking the finger.")
};


/**
 * 
 */
UCLASS(Abstract)
class MUTHM_API URhythm : public UInstruction , public IScoreInfo
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite)
		class URhythmWidgetBase* _CachedRhythmWidget=nullptr;

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
	UPROPERTY(BlueprintReadWrite)
		float WidthPercent = 0.2;

	UFUNCTION()
		void OnRhythmNumberDetailsChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
	UFUNCTION(BlueprintNativeEvent)
		TSubclassOf<URhythmWidgetBase> GetRhythmWidgetClass();
public:

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		ERhythmTouchResult OnTouchBegin(float X,float YPercent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		ERhythmTouchResult OnTouchTracking(float X, float YPercent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnTouchEnd(float X, float YPercent);

	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args) override;
	virtual void OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EditorExtraInfo) override;
	
	virtual class UInstructionWidgetBase* GenInstructionWidget_Implementation() override;

	virtual void OnPrepare_Implementation() override;
	virtual bool IsInstructionReady_Implementation(float CurrentTime) const override;

	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;

	virtual FBlueprintJsonObject GenArgsJsonObject_Implementation() override;
};
