// Copyright (C) 2019 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ModInstanceBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable,Abstract)
class MUTHM_API UModInstanceBase : public UObject
{
	GENERATED_BODY()

	int ModID;
public:
	void SetModID(int NewModID)
	{
		ModID = NewModID;
	}
	UFUNCTION(BlueprintPure)
		int GetModID() const
	{
		return ModID;
	}
	UFUNCTION(BlueprintImplementableEvent)
		void OnModInit();
	UFUNCTION(BlueprintNativeEvent)
		bool OnModUnload();

	//Override GetWorld() To provide the WorldContext
	virtual class UWorld* GetWorld() const override;

};
