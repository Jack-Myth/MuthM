// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Classes/BlueprintJsonLibrary.h"
#include "Instruction.generated.h"

/**
 * The Base UClass of all MMS Instructions
 */
UCLASS(BlueprintType,Blueprintable,Abstract)
class MUTHM_API UInstruction:public UObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintGetter="GetTime",BlueprintSetter="SetTime")
		float InstructionTime;
	TSharedPtr<UWorld> CachedWorld;
public:
	void SetWorld(UWorld* WorldContext);
	UFUNCTION(BlueprintGetter)
		float GetTime();
	UFUNCTION(BlueprintSetter)
		void SetTime(float pNewTime);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionLoaded(FBlueprintJsonObject Args);
	UFUNCTION(BlueprintImplementableEvent)
		void OnPrepare();
	UFUNCTION(BlueprintImplementableEvent)
		void OnTick(float CurrentTime);
	UFUNCTION(BlueprintImplementableEvent)
		void OnInstructionDestroy();
	UFUNCTION(BlueprintImplementableEvent)
		void OnTimeArrived();

	//Override GetWorld() To provide the WorldContext
	virtual class UWorld* GetWorld() const override final
	{
		return CachedWorld.Get();
	}

};