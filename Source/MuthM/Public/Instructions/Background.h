// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Instruction.h"
#include "Background.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UBackground : public UInstruction
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
		TArray<FString> SupportedAnimation;
	UPROPERTY(BlueprintReadWrite)
		FString Operation = "Show";
	UPROPERTY(BlueprintReadWrite)
		FString ImageFileName;
	UPROPERTY(BlueprintReadWrite)
		FString TargetAnimation = "None";
	UPROPERTY(BlueprintReadWrite)
		float AnimationLength = 0.5f;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* BackgroundImage=nullptr;
	UPROPERTY(BlueprintReadWrite)
		class UMaterialInstanceDynamic* CachedDMI = nullptr;
	UPROPERTY(BlueprintReadOnly)
		float AnimationProgress = -1.f;
	UFUNCTION()
		void OnStrValueCallback(class UInstruction* InstructionInstance, FName PropertyName, FString StrValue);
	UFUNCTION()
		void OnSelectionValueCallback(class UInstruction* InstructionInstance, FName PropertyName, class UDetailInputCustomBase* CustomWidget);
	UFUNCTION()
		void OnNumberValueCallback(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
	UFUNCTION(BlueprintCallable)	
		void InitProperties(FBlueprintJsonObject Args);
public:
	UBackground();

	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;
	virtual void OnTimeArrived_Implementation() override;
	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args) override;
	virtual void OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EditorExtraInfo) override;
	virtual bool IsInstructionReady_Implementation(float CurrentTime) const override;
	virtual void OnTick_Implementation(float CurrentTime) override;
	virtual FBlueprintJsonObject GenArgsJsonObject_Implementation() override;
};
