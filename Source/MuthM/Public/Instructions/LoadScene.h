// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Instruction.h"
#include "LoadScene.generated.h"

/**
 * 
 */
UCLASS()
class ULoadScene : public UInstruction
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly)
		class ULevelStreamingDynamic* CachedOnArrivedLevel=nullptr;
	UPROPERTY(BlueprintReadWrite)
		FString OnLoadSceneName;
	UPROPERTY(BlueprintReadWrite)
		FString OnArrivedSceneName;
	UFUNCTION()
		void OnStringDetailsCallback(class UInstruction* InstructionInstance, FName PropertyName, FString StrValue);
public:
	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args) override;

	virtual void OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EEI) override;

	virtual void OnTimeArrived_Implementation() override;

	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;

	virtual FBlueprintJsonObject GenArgsJsonObject_Implementation();
};
