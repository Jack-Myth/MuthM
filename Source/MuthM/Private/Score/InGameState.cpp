// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InGameState.h"

AInGameState::AInGameState()
{
	_ScoreGradeRecord.FindOrAdd(EScoreGrade::SG_Perfect) = 0;
	_ScoreGradeRecord.FindOrAdd(EScoreGrade::SG_Great) = 0;
	_ScoreGradeRecord.FindOrAdd(EScoreGrade::SG_Safe) = 0;
	_ScoreGradeRecord.FindOrAdd(EScoreGrade::SG_Bad) = 0;
	_ScoreGradeRecord.FindOrAdd(EScoreGrade::SG_Miss) = 0;
}

void AInGameState::SubmitScore(float RealScore)
{
	_RealScore += RealScore;
	OnScoreUpdate.Broadcast(GetScore(), GetRealScore());
}

void AInGameState::SubmitCombo()
{
	_CurrentComboCount++;
	_MaxComboRecorded = _MaxComboRecorded < _CurrentComboCount ? _CurrentComboCount : _MaxComboRecorded;
	OnComboUpdate.Broadcast(GetCurrentCombo());
}

void AInGameState::BreakCombo()
{
	_CurrentComboCount = 0;
	OnComboUpdate.Broadcast(GetCurrentCombo());
}

void AInGameState::SubmitGrade(EScoreGrade ScoreGrade)
{
	_ScoreGradeRecord.FindOrAdd(ScoreGrade)++;
	OnScoreGradeUpdate.Broadcast(ScoreGrade);
}

void AInGameState::InitScoreInfo(TArray<TScriptInterface<IScoreInfo>> ScoreInfoCollection)
{
	_RealScore = 0;
	_CurrentComboCount = 0;
	_MaxComboRecorded = 0;
	_ComboTop = int32(ScoreInfoCollection.Num()*0.3);
	_MaxScore = 0;
	float ComboScoreRatio=0;
	for (int i=0;i<_ComboTop;i++)
	{
		ComboScoreRatio += ((float)i / _ComboTop);
		_MaxScore += ScoreInfoCollection[i]->RequestPlainMaxScore();
	}
	ComboScoreRatio += ScoreInfoCollection.Num() - _ComboTop;
	for (int i= _ComboTop;i<ScoreInfoCollection.Num();i++)
		_MaxScore+=ScoreInfoCollection[i]->RequestPlainMaxScore();
	_MaxComboScore = _MaxScore * 0.5 / ComboScoreRatio;
	_MaxScore += _MaxScore * 0.5;
	OnComboUpdate.Broadcast(0);
	OnScoreUpdate.Broadcast(0.f, 0.f);
}

void AInGameState::SaveScoreRecord()
{
	//TODO: Save Score To disk.
}

void AInGameState::UploadScoreRecord()
{
	//TODO:Upload Score Record.
	//It need to Use network.
}
