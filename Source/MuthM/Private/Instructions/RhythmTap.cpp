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
#include "InGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "InGameState.h"
#include "InstructionWidgetBase.h"
#include "MuthMBPLib.h"
#include "UIProvider.h"
#include "DetailInputColorBase.h"

#define LOCTEXT_NAMESPACE "MuthM"

void URhythmTap::OnNumberPropertyChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	if (PropertyName=="MaxScore")
	{
		MaxScore = NumberValue;
	}
}

void URhythmTap::InitProperty(FBlueprintJsonObject& Args)
{
	TSharedPtr<FJsonObject> JsonArgs = Args.Object;
	UMuthMBPLib::GetColorFromJson(RhythmColor, JsonArgs, "Color");
	double tmpWidth;
	if (JsonArgs->TryGetNumberField("Width", tmpWidth))
		WidthPercent = tmpWidth;
	else
		WidthPercent = 0.2f;
	double tmpMaxScore = 0;
	if (JsonArgs->TryGetNumberField("MaxScore", tmpMaxScore))
		MaxScore = tmpMaxScore;
	else
		MaxScore = 100;
}

void URhythmTap::OnColorUpdate(class UInstruction* InstructionInstance, FName PropertyName, class UDetailInputCustomBase* CustomWidget)
{
	if (PropertyName=="Color")
	{
		RhythmColor = Cast<UDetailInputColorBase>(CustomWidget)->GetColor();
	}
}

void URhythmTap::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	Super::OnInstructionLoaded_Implementation(Args);
	InitProperty(Args);
}

void URhythmTap::OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args,FEditorExtraInfo EditorExtraInfo)
{
	Super::OnInstructionLoaded_Editor_Implementation(Args, EditorExtraInfo);
	InitProperty(Args);
}

void URhythmTap::OnPrepare_Implementation()
{
	Super::OnPrepare_Implementation();
	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	RhythmObj = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(1000.f, LROffset*SceneHalfWidth, 0.f),FRotator(0,90.f,0));
	RhythmObj->SetMobility(EComponentMobility::Movable);
	RhythmObj->GetStaticMeshComponent()->SetStaticMesh(PlaneMesh);
	GetRhythmMaterial(RhythmTapMatDynamic);
	RhythmObj->GetStaticMeshComponent()->SetMaterial(0, RhythmTapMatDynamic);
	RhythmObj->SetActorScale3D(FVector(WidthPercent*SceneHalfWidth / 50.f, 0.1f, 1));
	RhythmObj->SetActorEnableCollision(false);
}

void URhythmTap::OnTick_Implementation(float CurrentTime)
{
	//Super::OnTick(CurrentTime);
	if (CurrentTime < GetTime())
	{
		FVector ActorLocation = RhythmObj->GetActorLocation();
		RhythmObj->SetActorLocation(FVector((GetTime() - CurrentTime)*Speed, ActorLocation.Y, ActorLocation.Z));
	}	
	else if (CurrentTime - GetTime() > CheckWidthScale*DefaultCheckWidthBad)
	{
		//Player Missed.
		auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(this));
		auto* ScoreCore = Cast<AInGameState>(UGameplayStatics::GetGameState(this));
		ScoreCore->BreakCombo();
		ScoreCore->SubmitGrade(EScoreGrade::SG_Miss);
		RhythmObj->Destroy();
		DestroySelf();
	}
	else
	{
		FVector ActorLocation = RhythmObj->GetActorLocation();
		SetAlpha(RhythmTapMatDynamic, FMath::Min(1 - (CurrentTime - GetTime()), 1.f));
		RhythmObj->SetActorLocation(FVector((GetTime() - CurrentTime) * 30, ActorLocation.Y, ActorLocation.Z));
	}
}

ERhythmTouchResult URhythmTap::OnTouchBegin_Implementation(float X, float YPercent)
{
	bool beenTouched=false;
	//Check Position First
	if (YPercent > LROffset - WidthPercent && YPercent < LROffset + WidthPercent)
	{
		//Player Touched it.
		if (FMath::Abs(GetTime() - GetScript()->GetGameTime()) < CheckWidthScale*DefaultCheckWidthPerfect)
		{
			//Perfect
			auto* ScoreCore = Cast<AInGameState>(UGameplayStatics::GetGameState(this));
			ScoreCore->SubmitScore(MaxScore);
			ScoreCore->SubmitCombo();
			ScoreCore->SubmitGrade(EScoreGrade::SG_Perfect);
			beenTouched = true;
		}
		else if (FMath::Abs(GetTime() - GetScript()->GetGameTime()) < CheckWidthScale*DefaultCheckWidthGreat)
		{
			//Great
			auto* ScoreCore = Cast<AInGameState>(UGameplayStatics::GetGameState(this));
			ScoreCore->SubmitScore(MaxScore*0.80f);
			ScoreCore->SubmitCombo();
			ScoreCore->SubmitGrade(EScoreGrade::SG_Great);
			beenTouched = true;
		}
		else if (FMath::Abs(GetTime() - GetScript()->GetGameTime()) < CheckWidthScale*DefaultCheckWidthSafe)
		{
			//Safe
			auto* ScoreCore = Cast<AInGameState>(UGameplayStatics::GetGameState(this));
			ScoreCore->SubmitScore(MaxScore*0.50f);
			ScoreCore->SubmitCombo();
			ScoreCore->SubmitGrade(EScoreGrade::SG_Miss);
			beenTouched = true;
		}
		else if (FMath::Abs(GetTime() - GetScript()->GetGameTime()) <= CheckWidthScale*DefaultCheckWidthBad)
		{
			//Bad
			auto* ScoreCore = Cast<AInGameState>(UGameplayStatics::GetGameState(this));
			ScoreCore->SubmitScore(MaxScore*0.30f);
			ScoreCore->SubmitCombo();
			ScoreCore->SubmitGrade(EScoreGrade::SG_Bad);
			beenTouched = true;
		}
	}
	if (beenTouched)
	{
		RhythmObj->Destroy();
		DestroySelf();
		return ERhythmTouchResult::RTR_Accepted;
	}
	return ERhythmTouchResult::RTR_Continue;
}

float URhythmTap::RequestPlainMaxScore_Implementation()
{
	return MaxScore;
}

FBlueprintJsonObject URhythmTap::GenArgsJsonObject_Implementation()
{
	FBlueprintJsonObject SuperJsonObj = Super::GenArgsJsonObject_Implementation();
	SuperJsonObj.Object->SetNumberField("LROffset", LROffset);
	SuperJsonObj.Object->SetNumberField("Width", WidthPercent);
	SuperJsonObj.Object->SetNumberField("MaxScore", MaxScore);
	UMuthMBPLib::SaveColorToJson(SuperJsonObj.Object, "Color", RhythmColor);
	return SuperJsonObj;
}

void URhythmTap::OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder)
{
	Super::OnBuildingDetails_Implementation(DetailsBuilder);
	FDetailCategoryStruct RhythmCategory;
	RhythmCategory.Title = "RhythmTap";
	RhythmCategory.DisplayTitle = LOCTEXT("RhythmTap", "RhythmTap");

	TSharedPtr<FDetailItemCustom> ColorItem = MakeShareable(new FDetailItemCustom());
	UDetailInputColorBase* ColorInputWidget = Cast<UDetailInputColorBase>(UUserWidget::CreateWidgetInstance(*GetWorld(),
		UUIProvider::Get(this)->GetDetailInputColor(), NAME_None));
	ColorInputWidget->SetColor(RhythmColor);
	ColorItem->CustomWidget = ColorInputWidget;
	ColorItem->Name = "Color";
	ColorItem->DisplayName = LOCTEXT("Color", "Color");
	ColorItem->InstructionInstance = this;
	ColorItem->DetailCallbackCustom.BindUFunction(this, "OnColorUpdate");
	RhythmCategory.ItemList.Add(ColorItem);

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