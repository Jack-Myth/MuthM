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
	void OnStringDetailsCallback(class UInstruction* InstructionInstance, FName PropertyName, FString StrValue);
public:
	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args) override;

	virtual void OnTimeArrived_Implementation() override;

	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;
};
