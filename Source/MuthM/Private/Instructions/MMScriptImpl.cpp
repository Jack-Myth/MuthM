// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MMScriptImpl.h"
#include "MuthMTypeHelper.h"
#include "FileHelper.h"

bool UMMScriptImpl::_DeserializeInternal(const uint8* _JsonStr)
{
	if (!FMemory::Memcmp(_JsonStr, "_MMS", 4))
	{
		UE_LOG(MMScript, Error, TEXT("Invalid MMScript Data!"));
		return false;
	}
	_JsonStr += 4;
	uint32 InstructionCount = MuthMTypeHelper::LoadIntFromData(_JsonStr);
	//TODO: UMMScriptImpl::_DeserializeInternal(const uint8* _JsonStr) Unimplemented
	while (InstructionCount--)
	{

	}
	//Remember to order the Array
	return true;
}

bool UMMScriptImpl::LoadFromFile(FString FileName)
{
	TArray<uint8> FileData;
	if (FFileHelper::LoadFileToArray(FileData, *FileName))
	{
		bool Succeed = LoadFromData(FileData);
		if (!Succeed)
			UE_LOG(MMScript, Error, TEXT("Error Occured while Load Script(%s)!"), *FileName);
		return Succeed;
	}
	return false;
}

bool UMMScriptImpl::LoadFromData(const TArray<uint8>& FileData)
{
	return _DeserializeInternal(FileData.GetData());
}

//TODO: The Remove function May have redundancy.
void UMMScriptImpl::RemoveInstructionByType(TSubclassOf<UInstruction> InstructionType, EInstructionDestroyReason Reason)
{
	for (auto it = _InstructionInstances.CreateIterator(); it; ++it)
	{
		if ((*it)->GetClass() == InstructionType)
		{
			UInstruction* tmppInstruction = *it;
			it.RemoveCurrent();
			tmppInstruction->OnInstructionDestroy(Reason);
			tmppInstruction->RemoveFromRoot();
			tmppInstruction->MarkPendingKill();
		}
	}
	for (auto it = _PreparedInstructionInstance.CreateIterator(); it; ++it)
	{
		if ((*it)->GetClass() == InstructionType)
		{
			UInstruction* tmppInstruction = *it;
			it.RemoveCurrent();
			tmppInstruction->OnInstructionDestroy(Reason);
			tmppInstruction->RemoveFromRoot();
			tmppInstruction->MarkPendingKill();
		}
	}
}

void UMMScriptImpl::RemoveInstruction(UInstruction* Instance, EInstructionDestroyReason Reason)
{
	for (auto it = _InstructionInstances.CreateIterator(); it; ++it)
	{
		if ((*it) == Instance)
		{
			UInstruction* tmppInstruction = *it;
			it.RemoveCurrent();
			tmppInstruction->OnInstructionDestroy(Reason);
			tmppInstruction->RemoveFromRoot();
			tmppInstruction->MarkPendingKill();
			return;
		}
	}
	for (auto it = _PreparedInstructionInstance.CreateIterator(); it; ++it)
	{
		if ((*it) == Instance)
		{
			UInstruction* tmppInstruction = *it;
			it.RemoveCurrent();
			tmppInstruction->OnInstructionDestroy(Reason);
			tmppInstruction->RemoveFromRoot();
			tmppInstruction->MarkPendingKill();
			return;
		}
	}
}

void UMMScriptImpl::Tick(float CurrentTime)
{
	//Find the Instructions that need to be prepared.
	for (auto it = _InstructionInstances.CreateIterator(); it; ++it)
	{
		//TODO: Find a suit way to Caculate Prepare Time.
		if (0)
		{
			UInstruction* TargetInstruction = *it;
			it.RemoveCurrent();
			_PreparedInstructionInstance.Add(TargetInstruction);
			TargetInstruction->OnPrepare();
		}
		else
			break;
		//Because the Array is ordered,if the first no need to be prepared, the second one will also be.
	}
	for (int i = 0; i < _PreparedInstructionInstance.Num(); i++)
		_PreparedInstructionInstance[i]->OnTick(CurrentTime);
}
