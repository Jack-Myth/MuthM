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
	bool bIsAutoDestroy = true;
	EPlayType _PlayType;
	void _Internal_CleanInstructions();
	void _SetPlayType(EPlayType _PlayType);
	UMMScriptImpl() = default;
public:
	virtual bool LoadFromFile(FString FileName) override;
	virtual bool LoadFromData(const TArray<uint8>& FileData) override;
	virtual	void RemoveInstructionByType(TSubclassOf<UInstruction> InstructionType, EInstructionDestroyReason Reason) override;
	virtual void RemoveInstruction(UInstruction* Instance, EInstructionDestroyReason Reason) override;
	void Tick(float CurrentTime);
	virtual float GetRemainingInstructionCount() override;
	virtual void SetAutoDestroy(bool NewAutoDestroy) override;
	virtual void Destroy() override;
	virtual TScriptInterface<IMMScript> GenPIEDuplicate() override;
	virtual TArray<uint8> Serialize() override;
	virtual float GetSuiltableDelay() override;

};
