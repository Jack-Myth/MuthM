// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Instruction.h"
#include "GlobalVariable.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UGlobalVariable : public UInstruction
{
	GENERATED_BODY()
protected:
	UFUNCTION()
		void OnSelectionInput(class UInstruction* InstructionInstance, FName PropertyName, class UDetailInputCustomBase* CustomWidget);
	UFUNCTION()
		void OnStringInput(class UInstruction* InstructionInstance, FName PropertyName, FString StrValue);
	UFUNCTION()
		void OnNumberInput(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
	FString Type;

	FString Key;
	float Number;
	FString StrValue;

	void InitProperties(FBlueprintJsonObject Args);
public:
	UGlobalVariable();

	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args) override;
	virtual void OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EditorExtraInfo)override;

	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;
	FBlueprintJsonObject GenArgsJsonObject_Implementation() override;

	virtual void OnPrepare_Implementation() override;
};
