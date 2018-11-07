// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionManager.h"


// Add default functionality here for any IInstructionManager functions that are not pure virtual.


TScriptInterface<IInstructionManager> IInstructionManager::Get()
{
	return GetMutableDefault<UInstructionManagerImpl>();
}

bool UInstructionManagerImpl::RegisterInstruction(const FName& InstructionName, const TSubclassOf<UInstruction>& InstructionClass, FInstructionRef& InstructionRef)
{
	FInstructionStack* InstStack = _InstructionMap.Find(InstructionName);
	if (InstStack)
	{
		//Check If the InstructionClass Is the child of the Original Class
		if (InstructionClass->IsChildOf(InstStack->ClassStack.Top()))
		{
			InstStack->ClassStack.Push(InstructionClass);
			InstructionRef.InstructionName = InstructionName;
			InstructionRef.RecordPtr = InstructionClass;
			return true;
		}
		return false;
	}
	else
	{
		FInstructionStack tmpStack;
		tmpStack.ClassStack.Push(InstructionClass);
		FInstructionStack& AddedStack = _InstructionMap.Add(InstructionName, tmpStack);
		InstructionRef.InstructionName = InstructionName;
		InstructionRef.RecordPtr = AddedStack.ClassStack.Top();
		return true;
	}
}

void UInstructionManagerImpl::UnregisterInstruction(const FInstructionRef InstructionRef)
{
	//Check Unprepared Instruction List
	for (auto it = _InstructionInstanceList.CreateIterator(); it; ++it)
	{
		if ((*it)->GetClass() == InstructionRef.RecordPtr)
		{
			(*it)->OnInstructionDestroy(EInstructionDestroyReason::IDR_Unregister);
			it.RemoveCurrent();
		}
	}
	//Check Prepared Instruction List
	for (auto it = _PreparedInstructionInstanceList.CreateIterator(); it; ++it)
	{
		if ((*it)->GetClass() == InstructionRef.RecordPtr)
		{
			(*it)->OnInstructionDestroy(EInstructionDestroyReason::IDR_Unregister);
			it.RemoveCurrent();
		}
	}
	auto _InstStack = _InstructionMap.Find(InstructionRef.InstructionName);
	if (_InstStack)
	{
		//Remove the Class equal to RecordPtr
		//The other class won't be effect.
		int32 ClassIndex = _InstStack->ClassStack.Find(InstructionRef.RecordPtr);
		if (ClassIndex!=INDEX_NONE)
		{
			//If ClassStack has no Class anymore ,Remove it.
			_InstStack->ClassStack.RemoveAt(ClassIndex);
			if (!_InstStack->ClassStack.Num())
				_InstructionMap.Remove(InstructionRef.InstructionName);
		}
	}
}

bool UInstructionManagerImpl::ActiveInstruction(FName InstructionName, FJsonObject& JsonArg)
{
	const auto* InstructionClass = _InstructionMap.Find(InstructionName);
	if (InstructionClass)
	{
		UInstruction* InstructionPtr = NewObject<UInstruction>(nullptr, InstructionClass->ClassStack.Top());
		_InstructionInstanceList.Add(InstructionPtr);
		FBlueprintJsonObject tmpBPJsonArg;
		tmpBPJsonArg.Object = MakeShareable<FJsonObject>(&JsonArg);
		InstructionPtr->OnInstructionLoaded(tmpBPJsonArg);
		return true;
	}
	return false;
}

bool UInstructionManagerImpl::DestroyInstructionInstance(UInstruction* InstructionInstance, EInstructionDestroyReason DestroyReason)
{
	if (_InstructionInstanceList.Find(InstructionInstance))
		_InstructionInstanceList.Remove(InstructionInstance);
	else if (_PreparedInstructionInstanceList.Find(InstructionInstance))
		_PreparedInstructionInstanceList.Remove(InstructionInstance);
	else
		return false;
	InstructionInstance->OnInstructionDestroy(DestroyReason);
	InstructionInstance->RemoveFromRoot();
	InstructionInstance->MarkPendingKill();
	return true;
}

void UInstructionManagerImpl::Tick(float CurrentTime)
{
	for (auto it = _InstructionInstanceList.CreateIterator(); it; ++it)
	{
		UInstruction* pInstructionInstance = *it;
		//TODO:Prepare? What is the suit time?
	}
	for (int i = 0; i < _PreparedInstructionInstanceList.Num(); i++)
		_PreparedInstructionInstanceList[i]->OnTick(CurrentTime);
}
