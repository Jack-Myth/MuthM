// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interface.h"
#include "Instruction.h"
#include "SubclassOf.h"
#include "MMScript.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MMScript, Log, All)

UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UMMScript : public UInterface
{
	GENERATED_BODY()

};

UENUM(BlueprintType)
enum EPlayType
{
	PT_Game UMETA(DisplayName = "Game"),
	PT_Editor UMETA(DisplayName = "Editor"),
	PT_PIE UMETA(DisplayName = "PIE")
};

class MUTHM_API IMMScript
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		virtual bool LoadFromFile(FString FileName) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool LoadFromData(const TArray<uint8>& FileData) = 0;
	virtual void AddInstruction(class UInstruction* InstructionInstance) = 0;
	virtual	void RemoveInstructionByType(TSubclassOf<UInstruction> InstructionType,EInstructionDestroyReason Reason) = 0;
	virtual void RemoveInstruction(UInstruction* Instance, EInstructionDestroyReason Reason) = 0;
	UFUNCTION(BlueprintCallable)
		virtual float GetRemainingInstructionCount() const = 0;
	UFUNCTION(BlueprintCallable)
		virtual void SetAutoDestroy(bool NewAutoDestroy) = 0;
	UFUNCTION(BlueprintCallable)
		virtual void Destroy() = 0;
	virtual TArray<uint8> SerializeToData() = 0;
	virtual float GetSuiltableDelay() = 0;
	virtual TArray<TScriptInterface<class IScoreInfo>> CollectScoreInfoArray() = 0;
	virtual void SetPlayType(EPlayType PlayType) = 0;
	UFUNCTION(BlueprintCallable)
		virtual void SetBeginTime(float BeginTime) = 0;
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Get All Instruction that is or is subclass of the Given Class"))
		virtual TArray<UInstruction*> GetAllPreparedInstructionByType(TSubclassOf<UInstruction> TargetClass) const = 0;
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Same as MuthMInGameMode::GetGameTime()"))
		virtual float GetGameTime() const = 0;
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Notify MMS to Reorder Instructions"))
		virtual void UpdateInstructions() = 0;
	virtual TArray<UInstruction*> GetAllInstructions() const = 0;
	virtual bool Import(FString FileName, TScriptInterface<class IChartImporter> Importer) = 0;
};

