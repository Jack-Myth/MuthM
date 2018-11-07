// // Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interface.h"
#include "MMScript.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UMMScript : public UInterface
{
	GENERATED_BODY()

};

class MUTHM_API IMMScript
{

};

/**
 * 
 */
UCLASS(NotBlueprintable,NotBlueprintType)
class MUTHM_API UMMScriptImpl : public UObject
{
	GENERATED_BODY()

	void _DeserializeInternal(char* _JsonStr);
public:
	UFUNCTION(BlueprintCallable)
		void LoadFromFile(FString FileName);
	UFUNCTION(BlueprintCallable)
		void LoadFromData(const TArray<uint8>& FileData);
};
