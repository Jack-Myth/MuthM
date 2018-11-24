// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MMScriptImpl.h"
#include "MuthMTypeHelper.h"
#include "FileHelper.h"
#include "JsonObject.h"
#include "JsonReader.h"
#include "JsonSerializer.h"
#include "InstructionManager.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMInEditorMode.h"
#include "JsonSerializerMacros.h"
#include "JsonWriter.h"

bool UMMScriptImpl::_DeserializeInternal(const uint8* _MMSStr)
{
	if (!FMemory::Memcmp(_MMSStr, "_MMS", 4))
	{
		UE_LOG(MMScript, Error, TEXT("Invalid MMScript Data!"));
		return false;
	}
	_MMSStr += 4;  //Length of "_MMS"
	_Internal_CleanInstructions();
	uint32 InstructionCount = MuthMTypeHelper::LoadIntFromData(_MMSStr);
	struct tmpInstructionList
	{
		float Time;
		FName InstructionName;
		FJsonObject Args;
	};
	TArray<tmpInstructionList> tmpICollection;
	while (InstructionCount--)
	{
		tmpInstructionList tmpIItem;
		tmpIItem.Time = MuthMTypeHelper::LoadFloatFromData(_MMSStr);
		_MMSStr += sizeof(uint32);
		auto Conversion = FUTF8ToTCHAR((const ANSICHAR*)_MMSStr);
		tmpIItem.InstructionName = Conversion.Get();
		_MMSStr += Conversion.Length() + 1;  //Include Terminate Character
		auto ConversionJson = FUTF8ToTCHAR((const ANSICHAR*)_MMSStr);
		TSharedRef<TJsonReader<TCHAR>> ResultJsonReader = TJsonReaderFactory<TCHAR>::Create(ConversionJson.Get());
		TSharedPtr<FJsonObject> tmpArgPtr = MakeShareable(&tmpIItem.Args);
		FJsonSerializer::Deserialize(ResultJsonReader, tmpArgPtr);
		_MMSStr += ConversionJson.Length() + 1;		//Include Terminate Character
		tmpICollection.Push(tmpIItem);
	}
	tmpICollection.Sort([](const tmpInstructionList& a, const tmpInstructionList& b)
		{
			return a.Time < b.Time;
		});
	for (int i = 0; i < tmpICollection.Num(); i++)
	{
		UInstruction* InstructionInstance = IInstructionManager::Get()->GenInstruction(
			tmpICollection[i].InstructionName, tmpICollection[i].Time, tmpICollection[i].Args);
		_InstructionInstances.Push(InstructionInstance);
	}
	return true;
}

void UMMScriptImpl::_Internal_CleanInstructions()
{
	for (auto it = _InstructionInstances.CreateIterator(); it; ++it)
	{
		UInstruction* tmppInstruction = *it;
		it.RemoveCurrent();
		tmppInstruction->OnInstructionDestroy(EInstructionDestroyReason::IDR_ScriptDeleted);
		tmppInstruction->RemoveFromRoot();
		tmppInstruction->MarkPendingKill();
		return;
	}
	for (auto it = _PreparedInstructionInstance.CreateIterator(); it; ++it)
	{
		UInstruction* tmppInstruction = *it;
		it.RemoveCurrent();
		tmppInstruction->OnInstructionDestroy(EInstructionDestroyReason::IDR_ScriptDeleted);
		tmppInstruction->RemoveFromRoot();
		tmppInstruction->MarkPendingKill();
		return;
	}
}

void UMMScriptImpl::_SetPlayType(EPlayType _PlayType)
{
	this->_PlayType = _PlayType;
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
	if (!GetRemainingInstructionCount())
		Destroy();
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
	if (!GetRemainingInstructionCount())
		Destroy();
}

void UMMScriptImpl::Tick(float CurrentTime)
{
	//Find the Instructions that need to be prepared.
	for (auto it = _InstructionInstances.CreateIterator(); it; ++it)
	{
		//TODO: Find a suit way to Calculate Prepare Time.
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

float UMMScriptImpl::GetRemainingInstructionCount()
{
	return _InstructionInstances.Num() + _PreparedInstructionInstance.Num();
}

void UMMScriptImpl::Destroy()
{
	_Internal_CleanInstructions();
	IInstructionManager::Get()->DestroyMMScriptInstance(this);
}

TScriptInterface<IMMScript> UMMScriptImpl::GenPIEDuplicate()
{
	auto* InEditorMode = Cast<AMuthMInEditorMode>(UGameplayStatics::GetGameMode(this));
	if (InEditorMode&&_PlayType==EPlayType::PT_Editor)
	{
		UMMScriptImpl* curTmpMMS = Cast<UMMScriptImpl>(IInstructionManager::Get()->GenMMScript(true).GetObject());
		curTmpMMS->_SetPlayType(PT_PIE);
		curTmpMMS->LoadFromData(Serialize());
		return curTmpMMS;
	}
	return nullptr;
}

TArray<uint8> UMMScriptImpl::Serialize()
{
	TArray<uint8> MMSData;
	MMSData.Append((const uint8*)"_MMS", 4);
	MMSData.SetNum(4 + sizeof(uint32)); //"_MMS" and Instruction Count(sizeof(uint32)=4)
	//Reserve 4 byte for Instruction Count.
	TScriptInterface<IInstructionManager> InstructionManager=IInstructionManager::Get();
	//Get Args form Each Instructions.
	for (int i=0;i<_InstructionInstances.Num();i++)
	{
		FBlueprintJsonObject ArgsJsonObj = _InstructionInstances[i]->GenArgsJsonObject();
		FString InstructionJsonStr;
		FJsonObject InstructionJson;
		InstructionJson.SetNumberField("Time", _InstructionInstances[i]->GetTime());
		InstructionJson.SetStringField("Instruction",InstructionManager->GetInstructionName(_InstructionInstances[i]->GetClass()).ToString());
		InstructionJson.SetObjectField("Args", ArgsJsonObj.Object);
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&InstructionJsonStr);
		FJsonSerializer::Serialize(ArgsJsonObj.Object.ToSharedRef(), JsonWriter);
		FTCHARToUTF8 InstructionJsonStrConversion(*InstructionJsonStr);
		MMSData.Append(MuthMTypeHelper::SaveIntToData(InstructionJsonStrConversion.Length()));
		MMSData.Append((const uint8*)InstructionJsonStrConversion.Get(), InstructionJsonStrConversion.Length());
	}
	//Fill Count of Instructions.
	FMemory::Memcpy(MMSData.GetData() + 4, MuthMTypeHelper::SaveIntToData(_InstructionInstances.Num()).GetData(),sizeof(uint32));
	return MMSData;
}

float UMMScriptImpl::GetSuiltableDelay()
{
	//TODO: Caculate suit able Delay When Script Start;
	//It should not less than 0;
	return 0;
}

TArray<TScriptInterface<class IScoreInfo>> UMMScriptImpl::CollectScoreInfoArray()
{
	TArray<TScriptInterface<class IScoreInfo>> tmpCollection;
	for (int i=0;i<_InstructionInstances.Num();i++)
	{
		if (_InstructionInstances[i]->Implements<IScoreInfo>())
			tmpCollection.Add(_InstructionInstances[i]);
	}
	for (int i = 0; i < _PreparedInstructionInstance.Num(); i++)
	{
		if (_PreparedInstructionInstance[i]->Implements<IScoreInfo>())
			tmpCollection.Add(_PreparedInstructionInstance[i]);
	}
	return tmpCollection;
}

void UMMScriptImpl::SetAutoDestroy(bool NewAutoDestroy)
{
	bIsAutoDestroy = NewAutoDestroy;
}