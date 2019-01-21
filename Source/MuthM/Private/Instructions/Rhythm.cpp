// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "Rhythm.h"
#include "MMScript.h"

bool URhythm::OnBeginTouched_Implementation(float X, float Y)
{
	return false;
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
