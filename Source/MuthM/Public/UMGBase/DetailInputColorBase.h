// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UMGBase/DetailInputCustomBase.h"
#include "DetailInputColorBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UDetailInputColorBase : public UDetailInputCustomBase
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintCallable)
		FString GetRGBAHexFromColor(const FLinearColor& Color) const;
	UFUNCTION(BlueprintCallable)
		FLinearColor GetColorFromRGBAHex(const FString& RGBAHex) const;
public:
	UFUNCTION(BlueprintImplementableEvent)
		void SetColor(FLinearColor Color);
	UFUNCTION(BlueprintImplementableEvent)
		FLinearColor GetColor() const;
};
