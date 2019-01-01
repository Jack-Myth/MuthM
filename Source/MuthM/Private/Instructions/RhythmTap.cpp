// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "RhythmTap.h"
#include "Engine/StaticMesh.h"
#include "JsonObject.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

void URhythmTap::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	TSharedPtr<FJsonObject> JsonArgs = Args.Object;
	LROffset = JsonArgs->GetNumberField("LROffset");
	TSharedPtr<FJsonObject> ColorObj = JsonArgs->GetObjectField("Color");
	RhythmColor.R = ColorObj->GetNumberField("R") * 255;
	RhythmColor.G = ColorObj->GetNumberField("G") * 255;
	RhythmColor.B = ColorObj->GetNumberField("B") * 255;
	RhythmColor.A = ColorObj->GetNumberField("A") * 255;
}

void URhythmTap::OnPrepare_Implementation()
{
	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	//Need find a value for MaxLR
	float MaxLR = 500;
	RhythmObj = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(1000.f, LROffset*MaxLR, 10.f),FRotator(0,90.f,0));
	RhythmObj->SetMobility(EComponentMobility::Movable);
}

void URhythmTap::OnTick_Implementation(float CurrentTime)
{
	RhythmObj->SetActorLocation(FVector((GetTime()-CurrentTime)*GetGlobalNumberData("_MMSpeed")));
}

void URhythmTap::GetRhythmMaterial_Implementation(class UMaterialInterface*& ReturnedMaterial)
{
	UMaterial* RhythmTapMat = LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/MuthM/Materials/Game/RhythmTapMaterial.RhythmTapMaterial'"));
	UMaterialInstanceDynamic* RhythmTapMatDynamic = UMaterialInstanceDynamic::Create(RhythmTapMat, this);
	RhythmTapMatDynamic->SetVectorParameterValue("RhythmColor", RhythmColor);
	ReturnedMaterial = RhythmTapMatDynamic;
}
