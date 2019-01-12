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
#include "InstructionWidgetBase.h"

#define LOCTEXT_NAMESPACE "MuthM"

void URhythmTap::OnNumberPropertyChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	if (PropertyName=="LROffset")
	{
		LROffset = NumberValue;
		if (_CachedInstructionWidget)
			_CachedInstructionWidget->SetVerticalOffset(LROffset);
	}
	else if (PropertyName=="Width")
	{
		Width = NumberValue;
	}
	else if (PropertyName=="MaxScore")
	{
		MaxScore = NumberValue;
	}
}

void URhythmTap::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	TSharedPtr<FJsonObject> JsonArgs = Args.Object;
	LROffset = JsonArgs->GetNumberField("LROffset");
	TSharedPtr<FJsonObject> ColorObj = JsonArgs->GetObjectField("Color");
	RhythmColor.R = ColorObj->GetNumberField("R");
	RhythmColor.G = ColorObj->GetNumberField("G");
	RhythmColor.B = ColorObj->GetNumberField("B");
	RhythmColor.A = ColorObj->GetNumberField("A");
	double tmpWidth;
	if (JsonArgs->TryGetNumberField("Width", tmpWidth))
		Width = tmpWidth;
	else
		Width = 0.2f;
	double tmpMaxScore = 0;
	if (JsonArgs->TryGetNumberField("MaxScore", tmpMaxScore))
		MaxScore = tmpMaxScore;
	else
		MaxScore = 100;
}

void URhythmTap::OnInstructionLoaded_EditorExtra_Implementation(FEditorExtraInfo EditorExtraInfo)
{
	LROffset = EditorExtraInfo.VerticalOffset;
}

void URhythmTap::OnPrepare_Implementation()
{
	Super::OnPrepare_Implementation();
	CheckWidthScale = GetGlobalNumberData("_MUTHM_3DDROP_CHECK_WIDTH_SCALE");
	CheckWidthScale = CheckWidthScale == 0 ? 1.f : CheckWidthScale; //Reset if Scale not exist.
	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	RhythmObj = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(1000.f, LROffset*SceneHalfWidth, 10.f),FRotator(0,90.f,0));
	RhythmObj->SetMobility(EComponentMobility::Movable);
	GetRhythmMaterial(RhythmTapMatDynamic);
	RhythmObj->GetStaticMeshComponent()->SetMaterial(0, RhythmTapMatDynamic);
	RhythmObj->SetActorScale3D(FVector(Width*SceneHalfWidth / 50, 1, 1));
	RhythmObj->SetActorEnableCollision(false);
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

FBlueprintJsonObject URhythmTap::GenArgsJsonObject_Implementation()
{
	FBlueprintJsonObject SuperJsonObj = Super::GenArgsJsonObject_Implementation();
	SuperJsonObj.Object->SetNumberField("LROffset", LROffset);
	SuperJsonObj.Object->SetNumberField("Width", Width);
	SuperJsonObj.Object->SetNumberField("MaxScore", MaxScore);
	TSharedPtr<FJsonObject> ColorObj = MakeShareable(new FJsonObject());
	ColorObj->SetNumberField("R", RhythmColor.R);
	ColorObj->SetNumberField("G", RhythmColor.G);
	ColorObj->SetNumberField("B", RhythmColor.B);
	ColorObj->SetNumberField("A", RhythmColor.A);
	SuperJsonObj.Object->SetObjectField("Color", ColorObj);
	return SuperJsonObj;
}

void URhythmTap::OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder)
{
	Super::OnBuildingDetails_Implementation(DetailsBuilder);
	FDetailCategoryStruct RhythmCategory;
	RhythmCategory.Title = "RhythmTap";
	RhythmCategory.DisplayTitle = LOCTEXT("RhythmTap", "RhythmTap");
	TSharedPtr<FDetailItemNumber> LROffsetDetail = MakeShareable(new FDetailItemNumber());
	LROffsetDetail->Name = "LROffset";
	LROffsetDetail->DisplayName = LOCTEXT("PositionOffset", "Position Offset");
	LROffsetDetail->InstructionInstance = this;
	LROffsetDetail->NumberValue = LROffset;
	LROffsetDetail->SlideMax = 1.f;
	LROffsetDetail->SlideMin = -1.f;
	LROffsetDetail->DetailCallbackNumber.BindUFunction(this, "OnNumberPropertyChanged");
	RhythmCategory.ItemList.Add(LROffsetDetail);	

	TSharedPtr<FDetailItemNumber> WidthDetail = MakeShareable(new FDetailItemNumber());
	WidthDetail->Name = "Width";
	WidthDetail->DisplayName = LOCTEXT("Width", "Width");
	WidthDetail->InstructionInstance = this;
	WidthDetail->NumberValue = Width;
	WidthDetail->SlideMax = 1.f;
	WidthDetail->SlideMin = 0.1f;
	WidthDetail->DetailCallbackNumber.BindUFunction(this, "OnNumberPropertyChanged");
	RhythmCategory.ItemList.Add(WidthDetail);

	TSharedPtr<FDetailItemNumber> MaxScoreDetail = MakeShareable(new FDetailItemNumber());
	MaxScoreDetail->Name = "MaxScore";
	MaxScoreDetail->DisplayName = LOCTEXT("MaxScore", "MaxScore");
	MaxScoreDetail->InstructionInstance = this;
	MaxScoreDetail->NumberValue = MaxScore;
	MaxScoreDetail->DetailCallbackNumber.BindUFunction(this, "OnNumberPropertyChanged");
	RhythmCategory.ItemList.Add(MaxScoreDetail);
	DetailsBuilder->AddCategory(RhythmCategory);
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

#undef LOCTEXT_NAMESPACE