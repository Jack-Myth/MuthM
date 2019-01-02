// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "InstructionManagerImpl.h"
#include "MMScriptImpl.h"
#include "MuthMInEditorMode.h"
#include "Kismet/GameplayStatics.h"

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
	/*for (auto it = _InstructionInstanceList.CreateIterator(); it; ++it)
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
	}*/
	for (int i=0;i<_MMScriptInstances.Num();i++)
		_MMScriptInstances[i]->RemoveInstructionByType(InstructionRef.RecordPtr,EInstructionDestroyReason::IDR_Unregister);
	auto _InstStack = _InstructionMap.Find(InstructionRef.InstructionName);
	if (_InstStack)
	{
		//Remove the Class equal to RecordPtr
		//The other class won't be effect.
		int32 ClassIndex = _InstStack->ClassStack.Find(InstructionRef.RecordPtr);
		if (ClassIndex != INDEX_NONE)
		{
			//If ClassStack has no Class anymore ,Remove it.
			_InstStack->ClassStack.RemoveAt(ClassIndex);
			if (!_InstStack->ClassStack.Num())
				_InstructionMap.Remove(InstructionRef.InstructionName);
		}
	}
}

UInstruction* UInstructionManagerImpl::GenInstruction(FName InstructionName, float Time, FJsonObject& JsonArg)
{
	const auto* InstructionClass = _InstructionMap.Find(InstructionName);
	if (InstructionClass)
	{
		UInstruction* InstructionPtr = NewObject<UInstruction>(this);
		FBlueprintJsonObject tmpBPJsonArg;
		tmpBPJsonArg.Object = MakeShareable<FJsonObject>(&JsonArg);
		InstructionPtr->SetTime(Time);
		InstructionPtr->OnInstructionLoaded(tmpBPJsonArg);
		return InstructionPtr;
	}
	return nullptr;
}

void UInstructionManagerImpl::Tick(float CurrentTime)
{
	//TODO:UInstructionManagerImpl.Tick
	if (LastTickTime > CurrentTime)
		LastTickTime = 0;
	for (int i = 0; i < _MMScriptInstances.Num(); i++)
	{
		_MMScriptInstances[i]->Tick(CurrentTime);
	}
}

void UInstructionManagerImpl::DestroyMMScriptInstance(TScriptInterface<IMMScript> TargetMMSInstance)
{
	uint32 FoundIndex = _MMScriptInstances.Find(Cast<UMMScriptImpl>(TargetMMSInstance.GetObject()));
	if (FoundIndex != INDEX_NONE)
	{
		UMMScriptImpl* PendingKillMMS = _MMScriptInstances[FoundIndex];
		_MMScriptInstances.RemoveAt(FoundIndex);
		PendingKillMMS->RemoveFromRoot();
		PendingKillMMS->MarkPendingKill();
	}
}

TScriptInterface<IMMScript> UInstructionManagerImpl::GenMMScript(bool bIsEditorMode)
{
	UMMScriptImpl* pMMScript = NewObject<UMMScriptImpl>(this);
	if (bIsEditorMode)
	{
		pMMScript->SetPlayType(EPlayType::PT_Editor);
		_EditorMMSInstances.Add(pMMScript);
	}
	else
	{
		_MMScriptInstances.Add(pMMScript);
	}
	return pMMScript;
}

FName UInstructionManagerImpl::GetInstructionName(FInstructionRef InstructionRef) const
{
	return InstructionRef.InstructionName;
}

FName UInstructionManagerImpl::GetInstructionName(TSubclassOf<UInstruction> InstructionClass) const
{
	for (auto it=_InstructionMap.CreateConstIterator();it;++it)
	{
		if (it->Value.ClassStack.Find(InstructionClass) != INDEX_NONE)
			return it->Key;
	}
	return FName();
}

TScriptInterface<IMMScript> UInstructionManagerImpl::K2_GenMMScript()
{
	AGameModeBase* CurGameMode = UGameplayStatics::GetGameMode(this);
	return GenMMScript(CurGameMode->GetClass()->IsChildOf<AMuthMInEditorMode>());
}
