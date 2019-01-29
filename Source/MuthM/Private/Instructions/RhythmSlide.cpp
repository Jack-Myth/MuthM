// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "RhythmSlide.h"
#include "JsonObject.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/SplineMeshActor.h"
#include "Components/SplineMeshComponent.h"
#include "Regex.h"
#include "InGameMode.h"
#include "InGameState.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "MuthM"

void URhythmSlide::RhythmSlideNumberCallback(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	if (PropertyName=="SubNotesCount")
	{
		SubNotes.SetNum(FMath::Max((int)NumberValue, 0));
		//TODO: Update Widget.
	}
}

void URhythmSlide::SubNoteNumberCallback(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	FRegexPattern pattern("SubNote(\\d)-(.*)");
	FRegexMatcher matcher(pattern,PropertyName.ToString());
	if (!matcher.FindNext())
		return;
	FString SubNoteIndexStr = matcher.GetCaptureGroup(1);
	FString PropertyNameStr = matcher.GetCaptureGroup(2);
	int SubNoteIndex = FCString::Atoi(*SubNoteIndexStr);
	if (SubNoteIndex >= SubNotes.Num())
		return; //Index out of bound.
	if (PropertyNameStr == "TimeOffset")
	{
		SubNotes[SubNoteIndex].TimeOffset = NumberValue;
		//TODO: Update Widget
	}
	else if (PropertyNameStr == "PositionOffset")
	{
		SubNotes[SubNoteIndex].PositionOffset = NumberValue;
		//TODOL Update Widget
	}
	else if (PropertyNameStr == "Score")
	{
		SubNotes[SubNoteIndex].Score = NumberValue;
		//No need to update widget.
	}
}

void URhythmSlide::BreakDown()
{
	float ScoreCount = 0;
	float MaxScore = IScoreInfo::Execute_RequestPlainMaxScore(this);
	for (int i = 0; i <= CurrentSubNoteIndex; i++)
		ScoreCount += SubNotes[i].Score;
	auto* ScoreCore = Cast<AInGameState>(UGameplayStatics::GetGameState(this));
	ScoreCore->SubmitScore(ScoreCount);
	if (ScoreCount > MaxScore*0.8)
	{
		ScoreCore->SubmitGrade(EScoreGrade::SG_Great);
	}
	else if (ScoreCount > MaxScore*0.5f)
	{
		ScoreCore->SubmitGrade(EScoreGrade::SG_Safe);
	}
	else
	{
		ScoreCore->SubmitGrade(EScoreGrade::SG_Bad);
		ScoreCore->BreakCombo();
	}

	//Attach MainNote Obj
	RhythmMesh->AttachToActor(IntermediateActor, FAttachmentTransformRules::KeepWorldTransform);
	bBroken = true;
	bTracking = false;
	//TODO: Set Material.
}

void URhythmSlide::InitProperty(FBlueprintJsonObject Args)
{
	TSharedPtr<FJsonObject> Obj = Args.Object;
	int SubRhythmCount = Obj->GetIntegerField("SubNotesCount");
	for (int i = 0; i < SubRhythmCount; i++)
	{
		TSharedPtr<FJsonObject> SubNoteInfoJson = Obj->GetObjectField(FString::Printf(TEXT("SubNote%d"), i));
		if (SubNoteInfoJson)
		{
			FRhythmSlideSubNoteInfo tmpNoteInfo;
			tmpNoteInfo.TimeOffset = SubNoteInfoJson->GetNumberField("TimeOffset");
			tmpNoteInfo.PositionOffset = SubNoteInfoJson->GetNumberField("PositionOffset");
			tmpNoteInfo.Score = SubNoteInfoJson->GetNumberField("Score");
			TSharedPtr<FJsonObject> ColorJson = SubNoteInfoJson->GetObjectField("Color");
			if (ColorJson)
			{
				tmpNoteInfo.Color.R = ColorJson->GetNumberField("R");
				tmpNoteInfo.Color.G = ColorJson->GetNumberField("G");
				tmpNoteInfo.Color.B = ColorJson->GetNumberField("B");
				tmpNoteInfo.Color.A = ColorJson->GetNumberField("A");
			}
			SubNotes.Add(tmpNoteInfo);
		}
	}
}

ERhythmTouchResult URhythmSlide::OnTouchBegin_Implementation(float X, float YPercent)
{
	if (bBroken)
		return ERhythmTouchResult::RTR_Continue;
	if (YPercent > LROffset-WidthPercent&&YPercent<LROffset+WidthPercent)
	{
		float GameTime = GetScript()->GetGameTime();
		if (FMath::Abs(GetTime() - GameTime) < DefaultCheckWidthSafe)
		{
			//Pressed
			bTracking = true;
			return ERhythmTouchResult::RTR_Tracking;
		}
	}
	return ERhythmTouchResult::RTR_Continue;
}

ERhythmTouchResult URhythmSlide::OnTouchTracking_Implementation(float X, float YPercent)
{
	float GameTime = GetScript()->GetGameTime();
	if (CurrentSubNoteIndex+1 >= SubNotes.Num())
	{
		//If bigger than the last subnote
		auto* ScoreCore = Cast<AInGameState>(UGameplayStatics::GetGameState(this));
		ScoreCore->SubmitCombo();
		ScoreCore->SubmitScore(IScoreInfo::Execute_RequestPlainMaxScore(this));
		ScoreCore->SubmitGrade(EScoreGrade::SG_Perfect);
		bBroken = true;
		bTracking = false;
		RhythmMesh->AttachToActor(IntermediateActor, FAttachmentTransformRules::KeepWorldTransform);
		return ERhythmTouchResult::RTR_Accepted;
	}
	if (YPercent > PositionOffset - WidthPercent && YPercent < PositionOffset + WidthPercent)
	{
		return ERhythmTouchResult::RTR_Tracking;
	}
	else
	{
		BreakDown();
		return ERhythmTouchResult::RTR_Accepted;
	}
}

void URhythmSlide::OnTouchEnd_Implementation(float X, float YPercent)
{
	BreakDown();
}

float URhythmSlide::RequestPlainMaxScore_Implementation()
{
	float ScoreCount = 0.f;
	for (int i = 0; i < SubNotes.Num(); i++)
		ScoreCount += SubNotes[i].Score;
	return ScoreCount;
}

void URhythmSlide::OnPrepare_Implementation()
{
	Super::OnPrepare_Implementation();
	//Load StaticMeshs
	UStaticMesh* Plane = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	UStaticMesh* SubNoteStaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/MuthM/Meshs/CircleMesh.CircleMesh'"));
	//Create MainNote
	RhythmMesh = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(1000.f, LROffset*SceneHalfWidth, 0.f), FRotator(0, 90.f, 0));
	RhythmMesh->SetActorScale3D(FVector(WidthPercent*SceneHalfWidth / 50.f, 0.1f, 1));
	RhythmMesh->SetMobility(EComponentMobility::Movable);
	RhythmMesh->GetStaticMeshComponent()->SetStaticMesh(Plane);
	//Create SubNotes
	//Create intermediate Actor,Prevent SubNote's scale been effected.
	IntermediateActor = GetWorld()->SpawnActor<AActor>();
	IntermediateActor->SetRootComponent(NewObject<USceneComponent>(IntermediateActor));
	IntermediateActor->SetActorLocationAndRotation(RhythmMesh->GetActorLocation(),FRotator::ZeroRotator);
	for (int i=0;i<SubNotes.Num();i++)
	{
		//Create StaticMeshActor
		AStaticMeshActor* tmpSubNoteActor = GetWorld()->SpawnActor<AStaticMeshActor>();
		tmpSubNoteActor->SetMobility(EComponentMobility::Movable);
		tmpSubNoteActor->GetStaticMeshComponent()->SetStaticMesh(SubNoteStaticMesh);
		//TODO: Set Material
		tmpSubNoteActor->AttachToActor(IntermediateActor,FAttachmentTransformRules::KeepRelativeTransform);
		tmpSubNoteActor->SetActorRelativeScale3D(FVector(0.03f, 0.03f, 1.f));
		tmpSubNoteActor->SetActorRelativeLocation(FVector(Speed*SubNotes[i].TimeOffset, 2.f * SceneHalfWidth*SubNotes[i].PositionOffset, 0));
		//TODO:Set Color
		SubNotesMesh.Add(tmpSubNoteActor);

		//Create SplineMeshActor
		ASplineMeshActor* tmpSubNoteLine = GetWorld()->SpawnActor<ASplineMeshActor>();
		tmpSubNoteLine->SetMobility(EComponentMobility::Movable);
		tmpSubNoteLine->GetSplineMeshComponent()->SetStaticMesh(Plane);
		//TODO: Set Material
		tmpSubNoteLine->GetSplineMeshComponent()->SetStartScale(FVector2D(0.02f, 1.f));
		tmpSubNoteLine->GetSplineMeshComponent()->SetEndScale(FVector2D(0.02f, 1.f));
		tmpSubNoteLine->GetSplineMeshComponent()->SetStartPosition(i - 1 < 0 ? FVector(0) : FVector(Speed*SubNotes[i-1].TimeOffset, 2.f*SceneHalfWidth*SubNotes[i-1].PositionOffset,0));
		tmpSubNoteLine->GetSplineMeshComponent()->SetEndPosition(FVector(Speed*SubNotes[i].TimeOffset, 2.f*SceneHalfWidth*SubNotes[i].PositionOffset,0));
		tmpSubNoteLine->AttachToActor(IntermediateActor, FAttachmentTransformRules::KeepRelativeTransform);
		tmpSubNoteLine->SetActorRelativeLocation(FVector::ZeroVector);
		SubNotesLine.Add(tmpSubNoteLine);
	}
}

void URhythmSlide::OnTick_Implementation(float CurrentTime)
{
	//Check if GameTime bigger than next subnote's time;
	if (bTracking&&
		CurrentSubNoteIndex+1<SubNotes.Num()&&
		CurrentTime > GetTime() + SubNotes[CurrentSubNoteIndex+1].TimeOffset)
	{
		//Move to next SubNote.
		CurrentSubNoteIndex++;
	}
	if (CurrentSubNoteIndex + 1 < SubNotes.Num())
	{
		float LerpA = CurrentSubNoteIndex < 0 ? 0.f : SubNotes[CurrentSubNoteIndex].PositionOffset;
		float LerpB = SubNotes[CurrentSubNoteIndex + 1].PositionOffset;
		float TimeA = CurrentSubNoteIndex < 0 ? 0.f : SubNotes[CurrentSubNoteIndex].TimeOffset;
		float TimeC = CurrentTime - GetTime();
		float TimeB = SubNotes[CurrentSubNoteIndex + 1].TimeOffset;
		float LerpAlpha = (TimeC - TimeA) / (TimeB - TimeA);
		PositionOffset = LROffset + 2.f*FMath::Lerp(LerpA, LerpB, LerpAlpha);
		if (!bBroken)
		{
			FVector OriginalPosition = RhythmMesh->GetActorLocation();
			//If player pressed,Block at 0.f
			OriginalPosition.X = bTracking ? FMath::Max(Speed * (GetTime() - CurrentTime), 0.f) : Speed * (GetTime() - CurrentTime);
			OriginalPosition.Y = bTracking ? PositionOffset * SceneHalfWidth : OriginalPosition.Y;
			RhythmMesh->SetActorLocation(OriginalPosition);
		}
	}
	FVector IAOriginalPosition = IntermediateActor->GetActorLocation();
	IAOriginalPosition.X = Speed * (GetTime()-CurrentTime);
	IntermediateActor->SetActorLocation(IAOriginalPosition);
	if (CurrentTime > GetTime() + SubNotes.Last().TimeOffset + 1)
	{
		//Destroy all meshs.
		RhythmMesh->Destroy();
		for (int i = 0; i < SubNotesMesh.Num(); i++)
			SubNotesMesh[i]->Destroy();
		for (int i = 0; i < SubNotesLine.Num(); i++)
			SubNotesLine[i]->Destroy();
		IntermediateActor->Destroy();
		DestroySelf();
	}
	else if(!bBroken&&!bTracking&&CurrentTime>GetTime()+DefaultCheckWidthSafe)
	{
		//Missed
		auto* ScoreCore = Cast<AInGameState>(UGameplayStatics::GetGameState(this));
		ScoreCore->SubmitGrade(EScoreGrade::SG_Miss);
		ScoreCore->BreakCombo();
		RhythmMesh->AttachToActor(IntermediateActor, FAttachmentTransformRules::KeepWorldTransform);
		bBroken = true;
	}
}

void URhythmSlide::OnBuildingDetails_Implementation(TScriptInterface<IDetailsBuilder>& DetailsBuilder)
{
	Super::OnBuildingDetails_Implementation(DetailsBuilder);
	
	//Plain Category
	FDetailCategoryStruct RhythmSlide;
	RhythmSlide.Title = "RhythmSlide";
	RhythmSlide.DisplayTitle = LOCTEXT("RhythmSlide", "RhythmSlide");
	TSharedPtr<FDetailItemNumber> SubNotesCount= MakeShareable(new FDetailItemNumber());
	SubNotesCount->Name = "SubNotesCount";
	SubNotesCount->DisplayName = LOCTEXT("SubNotesCount", "SubNotesCount");
	SubNotesCount->InstructionInstance = this;
	SubNotesCount->NumberValue = SubNotes.Num();
	SubNotesCount->SlideMin = 1;
	SubNotesCount->SlideMax = 10;
	SubNotesCount->SlideUnit = 1.f;
	SubNotesCount->DetailCallbackNumber.BindUFunction(this, "RhythmSlideNumberCallback");
	RhythmSlide.ItemList.Add(SubNotesCount);
	DetailsBuilder->AddCategory(RhythmSlide);

	//SubNote Properties
	for (int i = 0; i < SubNotes.Num(); i++)
	{
		FDetailCategoryStruct tmpSubNoteCategory;
		tmpSubNoteCategory.Title = *FString::Printf(TEXT("SubNote%d"), i);
		tmpSubNoteCategory.DisplayTitle = FText::Format(LOCTEXT("SubNote{Index}", "SubNote{Index}"), i);
		//Time Offset
		TSharedPtr<FDetailItemNumber> tmpSubNoteTimeOffset = MakeShareable(new FDetailItemNumber());
		tmpSubNoteTimeOffset->Name= *FString::Printf(TEXT("SubNote%d-TimeOffset"), i);
		tmpSubNoteTimeOffset->DisplayName = LOCTEXT("TimeOffset", "TimeOffset");
		tmpSubNoteTimeOffset->NumberValue = SubNotes[i].TimeOffset;
		tmpSubNoteTimeOffset->SlideMin = 0.f;
		tmpSubNoteTimeOffset->SlideMax = 10.f;
		tmpSubNoteTimeOffset->DetailCallbackNumber.BindUFunction(this, "SubNoteNumberCallback");
		tmpSubNoteCategory.ItemList.Add(tmpSubNoteTimeOffset);
		//PositionOffset
		TSharedPtr<FDetailItemNumber> tmpSubNotePositionOffset = MakeShareable(new FDetailItemNumber());
		tmpSubNotePositionOffset->Name = *FString::Printf(TEXT("SubNote%d-PositionOffset"), i);
		tmpSubNotePositionOffset->DisplayName = LOCTEXT("PositionOffset", "PositionOffset");
		tmpSubNotePositionOffset->NumberValue = SubNotes[i].PositionOffset;
		tmpSubNotePositionOffset->SlideMin = 0.f;
		tmpSubNotePositionOffset->SlideMax = 10.f;
		tmpSubNotePositionOffset->DetailCallbackNumber.BindUFunction(this, "SubNoteNumberCallback");
		tmpSubNoteCategory.ItemList.Add(tmpSubNotePositionOffset);
		//Score
		TSharedPtr<FDetailItemNumber> tmpSubNoteScore = MakeShareable(new FDetailItemNumber());
		tmpSubNoteScore->Name = *FString::Printf(TEXT("SubNote%d-Score"), i);
		tmpSubNoteScore->DisplayName = LOCTEXT("ScoreOffset", "Score");
		tmpSubNoteScore->NumberValue = SubNotes[i].Score;
		tmpSubNoteScore->SlideMin = 0.f;
		tmpSubNoteScore->SlideMax = 100.f;
		tmpSubNoteScore->DetailCallbackNumber.BindUFunction(this, "SubNoteNumberCallback");
		tmpSubNoteCategory.ItemList.Add(tmpSubNoteScore);

		//TODO: Color

		DetailsBuilder->AddCategory(tmpSubNoteCategory);
	}
}

FBlueprintJsonObject URhythmSlide::GenArgsJsonObject_Implementation()
{
	FBlueprintJsonObject BPJsonObj = Super::GenArgsJsonObject_Implementation();
	TSharedPtr<FJsonObject> JsonObj = BPJsonObj.Object;
	JsonObj->SetNumberField("SubNotesCount", SubNotes.Num());
	for (int i=0;i<SubNotes.Num();i++)
	{
		TSharedPtr<FJsonObject> tmpSubNote=MakeShareable(new FJsonObject());
		tmpSubNote->SetNumberField("TimeOffset", SubNotes[i].TimeOffset);
		tmpSubNote->SetNumberField("PositionOffset", SubNotes[i].PositionOffset);
		tmpSubNote->SetNumberField("Score", SubNotes[i].Score);
		JsonObj->SetObjectField(FString::Printf(TEXT("SubNote%d"), i), tmpSubNote);
		//TODO: Add Color
	}
	return BPJsonObj;
}

void URhythmSlide::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	Super::OnInstructionLoaded_Implementation(Args);
	InitProperty(Args);
}

void URhythmSlide::OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EditorExtraInfo)
{
	Super::OnInstructionLoaded_Editor_Implementation(Args, EditorExtraInfo);
	InitProperty(Args);
}

#undef LOCTEXT_NAMESPACE