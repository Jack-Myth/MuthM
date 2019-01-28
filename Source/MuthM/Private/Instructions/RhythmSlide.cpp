// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "RhythmSlide.h"
#include "JsonObject.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/SplineMeshActor.h"
#include "Components/SplineMeshComponent.h"
#include "Regex.h"

#define LOCTEXT_NAMESPACE "MuthM"

void URhythmSlide::RhythmSlideNumberCallback(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{

}

void URhythmSlide::SubNoteNumberCallback(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	FRegexPattern pattern("SubNote[0-9]*-*");
	FRegexMatcher macher(pattern,PropertyName.ToString());
	FString SubNoteIndexStr= macher.GetCaptureGroup(0);
	FString PropertyNameStr = macher.GetCaptureGroup(1);
	int SubNoteIndex = FCString::Atoi(*SubNoteIndexStr);
	if (PropertyNameStr == "TimeOffset")
	{
		SubNotes[i].TimeOffset = NumberValue;
	}
}

void URhythmSlide::OnPrepare_Implementation()
{
	//Load StaticMeshs
	UStaticMesh* Plane = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	UStaticMesh* SubNoteStaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/MuthM/Meshs/CircleMesh.CircleMesh'"));
	//Create MainNote
	RhythmMesh = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(1000.f, LROffset*SceneHalfWidth, 10.f), FRotator(0, 90.f, 0));
	RhythmMesh->SetMobility(EComponentMobility::Movable);
	RhythmMesh->GetStaticMeshComponent()->SetStaticMesh(Plane);
	//Create SubNotes
	//Create intermediate Actor,Prevent SubNote's scale been effected.
	IntermediateActor = GetWorld()->SpawnActor<AActor>();
	IntermediateActor->SetActorLocation(RhythmMesh->GetActorLocation());
	for (int i=0;i<SubNotes.Num();i++)
	{
		//Create StaticMeshActor
		AStaticMeshActor* tmpSubNoteActor = GetWorld()->SpawnActor<AStaticMeshActor>();
		tmpSubNoteActor->SetMobility(EComponentMobility::Movable);
		tmpSubNoteActor->GetStaticMeshComponent()->SetStaticMesh(SubNoteStaticMesh);
		//TODO: Set Material
		tmpSubNoteActor->AttachToActor(IntermediateActor,FAttachmentTransformRules::KeepRelativeTransform);
		tmpSubNoteActor->SetActorRelativeScale3D(FVector(0.1f, 0.1f, 1.f));
		tmpSubNoteActor->SetActorRelativeLocation(FVector(Speed*SubNotes[i].TimeOffset, SceneHalfWidth*SubNotes[i].PositionOffset, 0));
		//TODO:Set Color

		//Create SplineMeshActor
		if (i+1>=SubNotes.Num())
			continue; //Ignore the last SubNote.
		ASplineMeshActor* tmpSubNoteLine = GetWorld()->SpawnActor<ASplineMeshActor>();
		tmpSubNoteLine->SetMobility(EComponentMobility::Movable);
		tmpSubNoteLine->GetSplineMeshComponent()->SetStaticMesh(Plane);
		//TODO: Set Material
		tmpSubNoteLine->GetSplineMeshComponent()->SetStartScale(FVector2D(0.1f, 1.f));
		tmpSubNoteLine->GetSplineMeshComponent()->SetEndScale(FVector2D(0.1f, 1.f));
		tmpSubNoteLine->GetSplineMeshComponent()->SetStartOffset(FVector2D(Speed*SubNotes[i].TimeOffset, SceneHalfWidth*SubNotes[i].PositionOffset));
		tmpSubNoteLine->GetSplineMeshComponent()->SetEndOffset(FVector2D(Speed*SubNotes[i + 1].TimeOffset, SceneHalfWidth*SubNotes[i + 1].PositionOffset));
		tmpSubNoteLine->AttachToActor(IntermediateActor, FAttachmentTransformRules::KeepRelativeTransform);
		tmpSubNoteLine->SetActorRelativeLocation(FVector::ZeroVector);
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
		tmpSubNoteScore->Name = *FString::Printf(TEXT("SubNote%d-ScoreOffset"), i);
		tmpSubNoteScore->DisplayName = LOCTEXT("ScoreOffset", "ScoreOffset");
		tmpSubNoteScore->NumberValue = SubNotes[i].Score;
		tmpSubNoteScore->SlideMin = 0.f;
		tmpSubNoteScore->SlideMax = 100.f;
		tmpSubNoteScore->DetailCallbackNumber.BindUFunction(this, "SubNoteNumberCallback");
		tmpSubNoteCategory.ItemList.Add(tmpSubNoteScore);

		//TODO: Color

		DetailsBuilder->AddCategory(tmpSubNoteCategory);
	}
}

void URhythmSlide::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	Super::OnInstructionLoaded_Implementation(Args);
	TSharedPtr<FJsonObject> Obj = Args.Object;
	int SubRhythmCount = Obj->GetIntegerField("SubNotesCount");
	for (int i=0;i<SubRhythmCount;i++)
	{
		TSharedPtr<FJsonObject> SubNoteInfoJson = Obj->GetObjectField(FString::Printf(TEXT("SubNote"), i));
		if (SubNoteInfoJson)
		{
			FRhythmPressSubNoteInfo tmpNoteInfo;
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

#undef LOCTEXT_NAMESPACE