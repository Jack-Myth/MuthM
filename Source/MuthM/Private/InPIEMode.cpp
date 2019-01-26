// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InPIEMode.h"
#include "MuthMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerInputHandler.h"

void AInPIEMode::NativeOnGameEnded(EGameEndReason GameEndReason)
{
	Super::NativeOnGameEnded(EGameEndReason::GER_ExitPIE);
	if (!Exiting)
	{
		Exiting = true;
		auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
		GameInstance->ExitPIEMode();
	}
}

void AInPIEMode::BindDelegates()
{
	auto* InputHandler = Cast<APlayerInputHandler>(UGameplayStatics::GetPlayerPawn(this, 0));
	InputHandler->OnBackPressed.AddUObject(this, &AInPIEMode::OnBackPressed);
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameInstance->OnExitPIE.AddUObject(this, &AInPIEMode::OnExitPIE);
}

void AInPIEMode::OnBackPressed()
{
	//Exit PIE
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameInstance->ExitPIEMode();
}

void AInPIEMode::OnExitPIE()
{
	if (!Exiting)
	{
		Exiting = true;
		NativeOnGameEnded(EGameEndReason::GER_ExitPIE);
	}
}

void AInPIEMode::BeginPlay()
{
	TargetPlayType = EPlayType::PT_PIE;
	Super::BeginPlay();
}
