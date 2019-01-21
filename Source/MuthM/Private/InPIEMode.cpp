// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InPIEMode.h"
#include "MuthMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerInputHandler.h"

void AInPIEMode::NativeOnGameEnded(EGameEndReason GameEndReason)
{
	Super::NativeOnGameEnded(EGameEndReason::GER_ExitPIE);
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameInstance->OnExitPIE.Broadcast();
}

void AInPIEMode::BindDelegates()
{
	auto* InputHandler = Cast<APlayerInputHandler>(UGameplayStatics::GetPlayerPawn(this, 0));
	InputHandler->OnBackPressed.AddUObject(this, &AInPIEMode::OnBackPressed);
}

void AInPIEMode::OnBackPressed()
{
	//Exit PIE
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameInstance->ExitPIEMode();
}
