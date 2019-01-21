// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MuthMGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API AMuthMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMuthMGameModeBase();
	UFUNCTION(BlueprintCallable)
		virtual void PauseGame() {};
	UFUNCTION(BlueprintCallable)
		virtual void ResumeGame() {};
protected:
	virtual void BeginPlay() override;
};
