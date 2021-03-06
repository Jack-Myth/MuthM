// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ScoreInfo.h"
#include "GameFramework/GameStateBase.h"
#include "InGameState.generated.h"

UENUM(BlueprintType)
enum class EScoreGrade :uint8
{
	SG_Perfect	UMETA(DisplayName="Perfect"),
	SG_Great	UMETA(DisplayName="Great"),
	SG_Safe		UMETA(DisplayName="Safe"),
	SG_Bad		UMETA(DisplayName="Bad"),
	SG_Miss		UMETA(DisplayName="Miss")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreUpdate, float, CurScore, float, CurRealScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboUpdate, int, CurCombo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreGradeUpdate, EScoreGrade, CurScoreGrade);

/**
 * 
 */
UCLASS()
class MUTHM_API AInGameState : public AGameStateBase
{
	GENERATED_BODY()

	float _RealScore;
	float _MaxScore;
	int32 _CurrentComboCount;
	int32 _MaxComboRecorded;
	int32 _ComboTop;
	float _MaxComboScore;
	TMap<EScoreGrade, int32> _ScoreGradeRecord;
public:
	UPROPERTY(BlueprintAssignable)
		FOnScoreUpdate OnScoreUpdate;
	UPROPERTY(BlueprintAssignable)
		FOnComboUpdate OnComboUpdate;
	UPROPERTY(BlueprintAssignable)
		FOnScoreGradeUpdate OnScoreGradeUpdate;
	AInGameState();
	UFUNCTION(BlueprintCallable)
		void SubmitScore(float RealScore);
	UFUNCTION(BlueprintCallable)
		void SubmitCombo();
	UFUNCTION(BlueprintCallable)
		void BreakCombo();
	UFUNCTION(BlueprintCallable)
		void SubmitGrade(EScoreGrade ScoreGrade);
	UFUNCTION(BlueprintPure)
		FORCEINLINE float GetScore()
	{
		return _RealScore / _MaxScore;
	}
	UFUNCTION(BlueprintPure)
		inline float GetRealScore()
	{
		return _RealScore;
	}
	UFUNCTION(BlueprintPure)
		inline float GetMaxScore()
	{
		return _MaxScore;
	}
	UFUNCTION(BlueprintPure)
		inline float GetCurrentCombo()
	{
		return _CurrentComboCount;
	}
	UFUNCTION(BlueprintPure)
		inline float GetMaxComboRecorded()
	{
		return _MaxComboRecorded;
	}
	UFUNCTION(BlueprintPure)
		inline TMap<EScoreGrade, int32> GetScoreGrades()
	{
		return _ScoreGradeRecord;
	}
	void InitScoreInfo(TArray<TScriptInterface<IScoreInfo>> ScoreInfoCollection);

	//No Need Score Info,Will get the score info from GameMode.
	void SaveScoreRecord();
	//No Need Score Info,Will get the score info from GameMode.
	void UploadScoreRecord();
};
