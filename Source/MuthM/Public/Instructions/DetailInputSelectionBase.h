// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UMGBase/DetailInputCustomBase.h"
#include "DetailInputSelectionBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UDetailInputSelectionBase : public UDetailInputCustomBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void FillOptions(const TArray<FString>& Options);
	UFUNCTION(BlueprintImplementableEvent)
		void SetCurrentSelection(const FString& CurrentSelction);
	UFUNCTION(BlueprintImplementableEvent)
		FString GetCurrentSelection();
};
