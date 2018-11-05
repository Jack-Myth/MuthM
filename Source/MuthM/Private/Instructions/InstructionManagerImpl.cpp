// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionManagerImpl.h"
#include "Instruction.h"
#include "Classes/BlueprintJsonLibrary.h"

bool UInstructionManagerImpl::RegisterInstruction(const FName& InstructionName,const TSubclassOf<UInstruction>& InstructionClass, FInstructionRef& InstructionRef)
{
	//TODO: Register Instruction
	if (_InstructionMap.Find(InstructionName))
		return false;
	TSubclassOf<UInstruction> tmppSubclass = _InstructionMap.Add(InstructionName, InstructionClass);
	InstructionRef.RecordPtr = tmppSubclass;
	return true;
}

void UInstructionManagerImpl::UnregisterInstruction(const FInstructionRef InstructionRef)
{
	//TODO: Unregister Instruction
	for (auto it = _InstructionInstanceList.CreateIterator();it;++it)
	{
		if ((*it)->GetClass()==InstructionRef.RecordPtr)
		{
			(*it)->OnInstructionDestroy(EInstructionDestroyReason::IDR_Unregister);
			it.RemoveCurrent();
		}
	}
}

bool UInstructionManagerImpl::ActiveInstruction(FName InstructionName, FJsonObject& JsonArg)
{
	const auto* InstructionClass = _InstructionMap.Find(InstructionName);
	if (InstructionClass)
	{
		UInstruction* InstructionPtr = NewObject<UInstruction>(nullptr, InstructionClass->Get());
		_InstructionInstanceList.Add(InstructionPtr);
		FBlueprintJsonObject tmpBPJsonArg;
		tmpBPJsonArg.Object = MakeShareable<FJsonObject>(&JsonArg);
		InstructionPtr->OnInstructionLoaded(tmpBPJsonArg);
		return true;
	}
	return false;
}

bool UInstructionManagerImpl::DestroyInstructionInstance(UInstruction* InstructionInstance,EInstructionDestroyReason DestroyReason)
{
	if (_InstructionInstanceList.Find(InstructionInstance))
	{
		_InstructionInstanceList.Remove(InstructionInstance);
		InstructionInstance->OnInstructionDestroy(DestroyReason);
		InstructionInstance->RemoveFromRoot();
		InstructionInstance->MarkPendingKill();
		return true;
	}
	return false;
}
