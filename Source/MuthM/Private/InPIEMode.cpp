// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InPIEMode.h"

void AInPIEMode::NativeOnGameEnded(EGameEndReason GameEndReason)
{
	Super::NativeOnGameEnded(EGameEndReason::GER_ExitPIE);
	OnExitPIEDelegate.ExecuteIfBound();
}
