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
}

bool URhythm::IsInstructionReady_Implementation() const
{
	return GetTime() < GetScript()->GetGameTime() - 10;
}
