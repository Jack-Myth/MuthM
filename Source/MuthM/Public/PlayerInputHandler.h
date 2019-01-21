// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerInputHandler.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBackPressed);

UCLASS()
class MUTHM_API APlayerInputHandler : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerInputHandler();
	FOnBackPressed OnBackPressed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnBackInput();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
