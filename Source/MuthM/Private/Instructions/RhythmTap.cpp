// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "RhythmTap.h"
#include "Engine/StaticMesh.h"
#include "JsonObject.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/BoxComponent.h"

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
}

void URhythmTap::OnPrepare_Implementation()
{
	//Super::OnPrepare_Implementation();
	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	//Need find a value for MaxLR
	float MaxLR = 500;
	RhythmObj = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(1000.f, LROffset*MaxLR, 10.f),FRotator(0,90.f,0));
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
	else if (CurrentTime - GetTime() > 1)
	{
		RhythmObj->Destroy();
		DestroySelf();
	}
	else
	{
		SetAlpha(RhythmTapMatDynamic, FMath::Max(1 - (CurrentTime - GetTime()), 1.f));
		RhythmObj->SetActorLocation(FVector((GetTime() - CurrentTime)*100));
	}
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
