// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Instruction.h"
#include "SubclassOf.h"
#include "JsonObject.h"
#include "InstructionManager.generated.h"

//Ref of Instruction,Hold by Blueprint7
USTRUCT(BlueprintType)
struct FInstructionRef
{
	GENERATED_BODY()
	FName InstructionName;
	TSubclassOf<UInstruction> RecordPtr;
};

USTRUCT()
struct FInstructionStack
{
	GENERATED_BODY()

	TArray<TSubclassOf<UInstruction>> ClassStack;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UInstructionManager : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MUTHM_API IInstructionManager
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	static TScriptInterface<IInstructionManager> Get();

	UFUNCTION(BlueprintCallable)
		virtual bool RegisterInstruction(const FName& InstructionName, const TSubclassOf<UInstruction>& InstructionClass, FInstructionRef& InstructionRef) = 0;

	UFUNCTION(BlueprintCallable)
		virtual void UnregisterInstruction(const FInstructionRef InstructionRef) = 0;

	virtual bool ActiveInstruction(FName InstructionName, FJsonObject& JsonArg) = 0;
	virtual bool DestroyInstructionInstance(UInstruction* InstructionInstance, EInstructionDestroyReason DestroyReason) = 0;
	virtual void Tick(float CurrentTime) = 0;
};

UCLASS(NotBlueprintable)
class MUTHM_API UInstructionManagerImpl : public UObject, public IInstructionManager
{
	GENERATED_BODY()

	UPROPERTY()
		TMap<FName, FInstructionStack> _InstructionMap;
	UPROPERTY()
		TArray<UInstruction*> _InstructionInstanceList;
	UPROPERTY()
		TArray<UInstruction*> _PreparedInstructionInstanceList;
public:
	virtual bool RegisterInstruction(const FName& InstructionName, const TSubclassOf<UInstruction>& InstructionClass, FInstructionRef& InstructionRef) override;
	virtual void UnregisterInstruction(const FInstructionRef InstructionRef) override;
	virtual bool ActiveInstruction(FName InstructionName, FJsonObject& JsonArg) override;

	//Called by the other function ,so DestroyReason must be provided.
	virtual bool DestroyInstructionInstance(UInstruction* InstructionInstance, EInstructionDestroyReason DestroyReason) override;
	virtual void Tick(float CurrentTime) override;

};