// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "RhythmTap.h"
#include "Engine/StaticMesh.h"
#include "JsonObject.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/BoxComponent.h"
#include "MMScript.h"
#include "MuthMInGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "ScoreCore.h"

void URhythmTap::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	TSharedPtr<FJsonObject> JsonArgs = Args.Object;
	LROffset = JsonArgs->GetNumberField("LROffset");
	TSharedPtr<FJsonObject> ColorObj = JsonArgs->GetObjectField("Color");
	RhythmColor.R = ColorObj->GetNumberField("R") * 255;
	RhythmColor.G = ColorObj->GetNumberField("G") * 255;
	RhythmColor.B = ColorObj->GetNumberField("B") * 255;
	RhythmColor.A = ColorObj->GetNumberField("A") * 255;
	Width = JsonArgs->GetNumberField("Width");
	float tmpMaxScore = JsonArgs->GetNumberField("MaxScore");
	if (tmpMaxScore)
		MaxScore = tmpMaxScore;
}

void URhythmTap::OnPrepare_Implementation()
{
	//Super::OnPrepare_Implementation();
	SceneHalfWidth = GetGlobalNumberData("_MUTHM_3DDROP_WIDTH") / 2.f;  //Convert to Half
	CheckWidthScale = GetGlobalNumberData("_MUTHM_3DDROP_CHECK_WIDTH");
	CheckWidthScale = CheckWidthScale == 0 ? 1.f : CheckWidthScale; //Reset if Scale not exist.
	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	RhythmObj = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(1000.f, LROffset*SceneHalfWidth, 10.f),FRotator(0,90.f,0));
	RhythmObj->SetMobility(EComponentMobility::Movable);
	GetRhythmMaterial(RhythmTapMatDynamic);
	RhythmObj->GetStaticMeshComponent()->SetMaterial(0, RhythmTapMatDynamic);
	RhythmObj->SetActorScale3D(FVector(Width, 1, 1));
	RhythmObj->SetActorEnableCollision(false);
	Speed = GetGlobalNumberData("_MMSpeed");
	if (!Speed)
		Speed = 1000;
}

void URhythmTap::OnTick_Implementation(float CurrentTime)
{
	//Super::OnTick(CurrentTime);
	if (CurrentTime<GetTime())
		RhythmObj->SetActorLocation(FVector((GetTime()-CurrentTime)*Speed));
	else if (CurrentTime - GetTime() > CheckWidthScale*DefaultCheckWidthBad)
	{
		//Player Missed.
		auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
		auto* ScoreCore = InGameMode->GetScoreCore();
		ScoreCore->BreakCombo();
		ScoreCore->SubmitGrade(EScoreGrade::SG_Miss);
		RhythmObj->Destroy();
		DestroySelf();
	}
	else
	{
		SetAlpha(RhythmTapMatDynamic, FMath::Max(1 - (CurrentTime - GetTime()), 1.f));
		RhythmObj->SetActorLocation(FVector((GetTime() - CurrentTime)*100));
	}
}

bool URhythmTap::OnBeginTouched_Implementation(float X, float Y)
{
	//Check Position First
	if (Y > LROffset / SceneHalfWidth - Width * 50 && Y < LROffset / SceneHalfWidth + Width * 50)
	{
		//Player Touched it.
		if (FMath::Abs(GetTime() - GetScript()->GetGameTime()) < CheckWidthScale*DefaultCheckWidthPerfect)
		{
			//Perfect
			auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
			auto* ScoreCore = InGameMode->GetScoreCore();
			ScoreCore->SubmitScore(MaxScore);
			ScoreCore->SubmitCombo();
			ScoreCore->SubmitGrade(EScoreGrade::SG_Perfect);
			return true;
		}
		else if (FMath::Abs(GetTime() - GetScript()->GetGameTime()) < CheckWidthScale*DefaultCheckWidthGreat)
		{
			//Great
			auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
			auto* ScoreCore = InGameMode->GetScoreCore();
			ScoreCore->SubmitScore(MaxScore*0.80f);
			ScoreCore->SubmitCombo();
			ScoreCore->SubmitGrade(EScoreGrade::SG_Great);
			return true;
		}
		else if (FMath::Abs(GetTime() - GetScript()->GetGameTime()) < CheckWidthScale*DefaultCheckWidthSafe)
		{
			//Safe
			auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
			auto* ScoreCore = InGameMode->GetScoreCore();
			ScoreCore->SubmitScore(MaxScore*0.50f);
			ScoreCore->SubmitCombo();
			ScoreCore->SubmitGrade(EScoreGrade::SG_Miss);
			return true;
		}
		else if (FMath::Abs(GetTime() - GetScript()->GetGameTime()) <= CheckWidthScale*DefaultCheckWidthBad)
		{
			//Bad
			auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(this));
			auto* ScoreCore = InGameMode->GetScoreCore();
			ScoreCore->SubmitScore(MaxScore*0.30f);
			ScoreCore->SubmitCombo();
			ScoreCore->SubmitGrade(EScoreGrade::SG_Bad);
			return true;
		}
	}
	return false;
}

float URhythmTap::RequestPlainMaxScore_Implementation()
{
	return MaxScore;
}

void URhythmTap::SetAlpha_Implementation(class UMaterialInstanceDynamic* RhythmDMI, float Alpha)
{
	RhythmDMI->SetScalarParameterValue("Alpha", Alpha);
}

void URhythmTap::GetRhythmMaterial_Implementation(class UMaterialInstanceDynamic*& ReturnedMaterial)
{
	UMaterial* RhythmTapMat = LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/MuthM/Materials/Game/RhythmTapMaterial.RhythmTapMaterial'"));
	UMaterialInstanceDynamic* _RhythmTapMatDynamic = UMaterialInstanceDynamic::Create(RhythmTapMat, this);
	_RhythmTapMatDynamic->SetVectorParameterValue("RhythmColor", RhythmColor);
	ReturnedMaterial = _RhythmTapMatDynamic;
}
