// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InPIEMode.h"
#include "MuthMGameInstance.h"
#include "Kismet/GameplayStatics.h"

void AInPIEMode::NativeOnGameEnded(EGameEndReason GameEndReason)
{
	Super::NativeOnGameEnded(EGameEndReason::GER_ExitPIE);
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameInstance->OnExitPIE.Broadcast();
}
