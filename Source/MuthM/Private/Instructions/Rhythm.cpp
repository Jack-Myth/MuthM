// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "Rhythm.h"
#include "MMScript.h"
#include "InstructionWidgetBase.h"
#include "JsonObject.h"

#define LOCTEXT_NAMESPACE "MuthM"

bool URhythm::OnBeginTouched_Implementation(float X, float Y)
{
	return false;
}

void URhythm::OnRhythmNumberDetailsChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	if (PropertyName == "LROffset")
	{
		LROffset = NumberValue;
		if (_CachedInstructionWidget) //Template doesn't have Instruction Widget
		{
			_CachedInstructionWidget->SetVerticalOffset(LROffset);
			_CachedInstructionWidget->RefreshOffset();
		}
	}
}

void URhythm::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	Super::OnInstructionLoaded_Implementation(Args);
	LROffset = Args.Object->GetNumberField("LROffset");
}

void URhythm::OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EditorExtraInfo)
{
	Super::OnInstructionLoaded_Implementation(Args);
	LROffset = Args.Object->GetNumberField("LROffset");
}

void URhythm::OnPrepare_Implementation()
{
	SceneHalfWidth = GetGlobalNumberData("_MUTHM_3DDROP_SCENE_WIDTH") / 2.f;  //Convert to Half
	Speed = GetGlobalNumberData("_MUTHM_GameSpeed");
	if (!Speed)
		Speed = 100.f;
	CheckWidthScale = GetGlobalNumberData("_MUTHM_3DDROP_CHECK_WIDTH_SCALE");
	CheckWidthScale = CheckWidthScale == 0 ? 1.f : CheckWidthScale; //Reset if Scale not exist.
}

bool URhythm::IsInstructionReady_Implementation() const
{
	//TODO: need to find a better solution.
	return GetTime() < GetScript()->GetGameTime() + 10;
}

void URhythm::OnBuildingDetails_Implementation(TScriptInterface<IDetailsBuilder>& DetailsBuilder)
{
	Super::OnBuildingDetails_Implementation(DetailsBuilder);

	FDetailCategoryStruct RhythmCategory;
	TSharedPtr<FDetailItemNumber> LROffsetDetail = MakeShareable(new FDetailItemNumber());
	LROffsetDetail->Name = "LROffset";
	LROffsetDetail->DisplayName = LOCTEXT("PositionOffset", "Position Offset");
	LROffsetDetail->InstructionInstance = this;
	LROffsetDetail->NumberValue = LROffset;
	LROffsetDetail->SlideMax = 1.f;
	LROffsetDetail->SlideMin = -1.f;
	LROffsetDetail->DetailCallbackNumber.BindUFunction(this, "OnRhythmNumberDetailsChanged");
	RhythmCategory.ItemList.Add(LROffsetDetail);
	
	DetailsBuilder->AddCategory(RhythmCategory);
}

#undef LOCTEXT_NAMESPACE