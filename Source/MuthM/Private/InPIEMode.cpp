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

#if 0
void AInPIEMode::TickPIE(float CurrentTime, float Duration)
{
	//Tick() is not a complete implementation, Only for Unreal Editor PIE!
	if (GIsEditor)
	{
		float DeltaTime = CurrentTime - LastTime;
		LastTime = CurrentTime;
		DeltaTime = DeltaTime ? DeltaTime : 0.016f;
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), AllActors);
		for (int i = 0; i < AllActors.Num(); i++)
			AllActors[i]->Tick(DeltaTime);
	}
}
#endif

void AInPIEMode::BindDelegates()
{
	auto* InputHandler = Cast<APlayerInputHandler>(UGameplayStatics::GetPlayerPawn(this, 0));
	InputHandler->OnBackPressed.AddUObject(this, &AInPIEMode::OnBackPressed);
	auto* GameInstance = Cast<UMuthMGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameInstance->OnExitPIE.AddUObject(this, &AInPIEMode::OnExitPIE);
#if 0
	OnMusicPlaybackTimeUpdate.AddDynamic(this, &AInPIEMode::TickPIE);
#endif
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
