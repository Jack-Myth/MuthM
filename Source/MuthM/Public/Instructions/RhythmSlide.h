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
	UPROPERTY(BlueprintReadWrite)
		FLinearColor Color = FLinearColor(0.4f, 0.8f, 1.f); //#66CCFF :P
};

/**
 * 
 */
UCLASS()
class MUTHM_API URhythmSlide : public URhythm
{
	GENERATED_BODY()
	
	bool bTracking = false;
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

	UPROPERTY(BlueprintReadWrite,meta=(ToolTip="Prevent to be clicked double times."))
		bool bBroken = false;
	UPROPERTY(BlueprintReadWrite)
		int CurrentSubNoteIndex = -1;
	void BreakDown();
	void InitProperty(FBlueprintJsonObject Args);
	UPROPERTY(BlueprintReadOnly)
		float PositionOffset=0.f;
public:
	virtual ERhythmTouchResult OnTouchBegin_Implementation(float X, float YPercent) override;
	virtual ERhythmTouchResult OnTouchTracking_Implementation(float X, float YPercent) override;
	virtual void OnTouchEnd_Implementation(float X, float YPercent) override;

	virtual float RequestPlainMaxScore_Implementation() override;

	virtual void OnPrepare_Implementation() override;

	virtual void OnTick_Implementation(float CurrentTime) override;

	virtual void OnBuildingDetails_Implementation(UPARAM(Ref) TScriptInterface<IDetailsBuilder>& DetailsBuilder) override;

	virtual FBlueprintJsonObject GenArgsJsonObject_Implementation() override;

	virtual void OnInstructionLoaded_Implementation(FBlueprintJsonObject Args) override;


	virtual void OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EditorExtraInfo) override;

};
