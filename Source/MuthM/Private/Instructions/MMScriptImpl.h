// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "MMScript.h"
#include "MMScriptImpl.generated.h"

/**
 *
 */
UCLASS(NotBlueprintable)
class MUTHM_API UMMScriptImpl : public UObject, public IMMScript
{
	GENERATED_BODY()
		friend class UInstructionManagerImpl;
	bool _DeserializeInternal(const uint8* _MMSStr);
	UPROPERTY()
		TArray<UInstruction*> _InstructionInstances;
	UPROPERTY()
		TArray<UInstruction*> _PreparedInstructionInstance;
	TArray<UInstruction*> _PriorityInstructions;
	bool bIsAutoDestroy = true;
	EPlayType _PlayType;
	void _Internal_CleanInstructions();
	UMMScriptImpl() = default;
	float mLastTime = 0;
protected:
	static TSharedPtr<FJsonObject> CollectEditorArgs(class UInstruction* Instruction);
	static void ApplyEditorArgs(TSharedPtr<FJsonObject>& EditorArgsJson, class UInstruction* Instruction);
public:
	virtual bool LoadFromFile(FString FileName) override;
	virtual bool LoadFromData(const TArray<uint8>& FileData) override;
	virtual	void RemoveInstructionByType(TSubclassOf<UInstruction> InstructionType, EInstructionDestroyReason Reason) override;
	virtual void RemoveInstruction(UInstruction* Instance, EInstructionDestroyReason Reason) override;
	void Tick(float CurrentTime);
	virtual float GetRemainingInstructionCount() const override;
	virtual void SetAutoDestroy(bool NewAutoDestroy) override;
	virtual void Destroy() override;
	virtual TArray<uint8> SerializeToData() override;
	virtual float GetSuiltableDelay() override;
	virtual TArray<TScriptInterface<class IScoreInfo>> CollectScoreInfoArray() override;
	virtual void SetBeginTime(float BeginTime) override;
	virtual void SetPlayType(EPlayType PlayType) override;
	virtual void AddInstruction(class UInstruction* InstructionInstance) override;
	virtual TArray<UInstruction *> GetAllPreparedInstructionByType(TSubclassOf<UInstruction> TargetClass) const override;
	virtual float GetGameTime() const override;
	virtual void UpdateInstructions() override;
	virtual TArray<UInstruction *> GetAllInstructions() const override;
	virtual bool Import(FString FileName, TScriptInterface<class IChartImporter> Importer) override;

};
