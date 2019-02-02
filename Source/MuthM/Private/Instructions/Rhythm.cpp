// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "Rhythm.h"
#include "MMScript.h"
#include "JsonObject.h"
#include "UIProvider.h"
#include "RhythmWidgetBase.h"

#define LOCTEXT_NAMESPACE "MuthM"

ERhythmTouchResult URhythm::OnTouchBegin_Implementation(float X, float YPercent)
{
	return ERhythmTouchResult::RTR_Continue;
}

void URhythm::OnRhythmNumberDetailsChanged(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	if (PropertyName == "LROffset")
	{
		LROffset = NumberValue;
		if (_CachedRhythmWidget) //Template doesn't have Instruction Widget
		{
			_CachedRhythmWidget->SetVerticalOffset(LROffset);
			_CachedRhythmWidget->RefreshOffset();
		}
	}
	else if (PropertyName == "Width")
	{
		WidthPercent = NumberValue;
		if (_CachedRhythmWidget)
		{
			_CachedRhythmWidget->SetWidthPercent(WidthPercent);
			_CachedRhythmWidget->RefreshOffset();
		}
	}
}

ERhythmTouchResult URhythm::OnTouchTracking_Implementation(float X, float YPercent)
{
	return ERhythmTouchResult::RTR_Accepted;
}

void URhythm::OnTouchEnd_Implementation(float X, float YPercent)
{}

void URhythm::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	Super::OnInstructionLoaded_Implementation(Args);
	LROffset = Args.Object->GetNumberField("LROffset");
}

void URhythm::OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EditorExtraInfo)
{
	Super::OnInstructionLoaded_Implementation(Args);
	LROffset = Args.Object->GetNumberField("LROffset");
	if (EditorExtraInfo.ExtraInfoValid)
	{
		LROffset = EditorExtraInfo.VerticalOffset;
	}
}

class UInstructionWidgetBase* URhythm::GenInstructionWidget_Implementation()
{
	auto RhythmWidgetClass = UUIProvider::Get(this)->GetRhythmWidget();
	_CachedRhythmWidget = Cast<URhythmWidgetBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), RhythmWidgetClass, NAME_None));
	_CachedRhythmWidget->SetVerticalOffset(LROffset);
	_CachedRhythmWidget->SetWidthPercent(WidthPercent);
	_CachedRhythmWidget->RefreshOffset();
	return _CachedRhythmWidget;
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

bool URhythm::IsInstructionReady_Implementation(float CurrentTime) const
{
	//TODO: need to find a better solution.
	return GetTime() < CurrentTime + 10;
}

void URhythm::OnBuildingDetails_Implementation(TScriptInterface<IDetailsBuilder>& DetailsBuilder)
{
	Super::OnBuildingDetails_Implementation(DetailsBuilder);

	FDetailCategoryStruct RhythmCategory;
	RhythmCategory.DisplayTitle = LOCTEXT("Rhythm", "Rhythm");
	TSharedPtr<FDetailItemNumber> LROffsetDetail = MakeShareable(new FDetailItemNumber());
	LROffsetDetail->Name = "LROffset";
	LROffsetDetail->DisplayName = LOCTEXT("PositionOffset", "Position Offset");
	LROffsetDetail->InstructionInstance = this;
	LROffsetDetail->NumberValue = LROffset;
	LROffsetDetail->SlideMax = 1.f;
	LROffsetDetail->SlideMin = -1.f;
	LROffsetDetail->DetailCallbackNumber.BindUFunction(this, "OnRhythmNumberDetailsChanged");
	RhythmCategory.ItemList.Add(LROffsetDetail);

	TSharedPtr<FDetailItemNumber> WidthDetail = MakeShareable(new FDetailItemNumber());
	WidthDetail->Name = "Width"; //It's actually not the really "width".
	WidthDetail->DisplayName = LOCTEXT("WidthPercent", "Width Percent");
	WidthDetail->InstructionInstance = this;
	WidthDetail->NumberValue = WidthPercent;
	WidthDetail->SlideMax = 1.f;
	WidthDetail->SlideMin = 0.1f;
	WidthDetail->DetailCallbackNumber.BindUFunction(this, "OnRhythmNumberDetailsChanged");
	RhythmCategory.ItemList.Add(WidthDetail);
	
	DetailsBuilder->AddCategory(RhythmCategory);
}

#undef LOCTEXT_NAMESPACE