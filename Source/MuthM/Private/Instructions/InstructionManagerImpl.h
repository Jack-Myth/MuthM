// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "InstructionManager.h"
#include "InstructionManagerImpl.generated.h"

UCLASS(NotBlueprintable, NotBlueprintType)
class MUTHM_API UInstructionManagerImpl : public UObject, public IInstructionManager
{
	GENERATED_BODY()

	UPROPERTY()
		TMap<FName, FInstructionStack> _InstructionMap;
	UPROPERTY()
		TArray<class UMMScriptImpl*> _MMScriptInstances;
	UPROPERTY()
		TArray<class UMMScriptImpl*> _EditorMMSInstances;
	UPROPERTY()
		UObject* _WorldContextProvider = nullptr;
	float LastTickTime;
public:
	virtual bool RegisterInstruction(const FName& InstructionName, const TSubclassOf<UInstruction>& InstructionClass, FInstructionRef& InstructionRef) override;
	virtual void UnregisterInstruction(const FInstructionRef InstructionRef) override;
	virtual UInstruction* GenInstruction(FName InstructionName, float Time) override;
	virtual void Tick(float CurrentTime) override;
	virtual void DestroyMMScriptInstance(TScriptInterface<IMMScript> TargetMMSInstance) override;
	virtual TScriptInterface<IMMScript> GenMMScript(bool bIsEditorMode = false) override;
	virtual FName GetInstructionName(FInstructionRef InstructionRef) const override;
	virtual FName GetInstructionName(TSubclassOf<UInstruction> InstructionClass) const override;
	virtual TScriptInterface<IMMScript> K2_GenMMScript() override;


	virtual TArray<FName> ListAllInstructionName() const override;

};