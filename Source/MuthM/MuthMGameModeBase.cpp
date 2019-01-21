// Fill out your copyright notice in the Description page of Project Settings.

#include "MuthMGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerInputHandler.h"

AMuthMGameModeBase::AMuthMGameModeBase()
{
	DefaultPawnClass = APlayerInputHandler::StaticClass();
}

void AMuthMGameModeBase::BeginPlay()
{
	APlayerController* pc = UGameplayStatics::GetPlayerController(this, 0);
	pc->bShowMouseCursor = true;
}
