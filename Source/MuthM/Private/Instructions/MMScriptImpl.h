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
public:
	virtual bool LoadFromFile(FString FileName) override;
	virtual bool LoadFromData(const TArray<uint8>& FileData) override;
	virtual	void RemoveInstructionByType(TSubclassOf<UInstruction> InstructionType, EInstructionDestroyReason Reason) override;
	virtual void RemoveInstruction(UInstruction* Instance, EInstructionDestroyReason Reason) override;
	void Tick(float CurrentTime);
};
