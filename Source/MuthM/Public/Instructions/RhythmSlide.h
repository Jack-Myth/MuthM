// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Instructions/Rhythm.h"
#include "RhythmSlide.generated.h"

USTRUCT(BlueprintType)
struct FRhythmSlideSubNoteInfo
{
	GENERATED_BODY()
		;
	UPROPERTY(BlueprintReadWrite)
		float TimeOffset=0.f;
	UPROPERTY(BlueprintReadWrite)
		float PositionOffset=0.f;
	UPROPERTY(BlueprintReadWrite)
		float Score = 50.f;
	UPROPERTY(BlueprintCallable)
		FLinearColor Color = FLinearColor(0.4f, 0.8f, 1.f); //#66CCFF :P
};

/**
 * 
 */
UCLASS()
class MUTHM_API URhythmSlide : public URhythm
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite)
		TArray<FRhythmSlideSubNoteInfo> SubNotes;
	UPROPERTY(BlueprintReadWrite)
		class AStaticMeshActor* RhythmMesh=nullptr;
	UPROPERTY(BlueprintReadWrite)
		class AActor* IntermediateActor=nullptr;
	UPROPERTY(BlueprintReadWrite)
		TArray<class AStaticMeshActor*> SubNotesMesh;
	UPROPERTY(BlueprintReadWrite)
		TArray<class ASplineMeshActor*> SubNotesLine;
	UFUNCTION()
		void RhythmSlideNumberCallback(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
	UFUNCTION()
		void SubNoteNumberCallback(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue);
public:
	virtual void OnPrepare_Implementation() override;


	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;


	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args) override;

};
