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
	UFUNCTION(BlueprintCallable)
		virtual void LoadFromFile(FString FileName) = 0;
	UFUNCTION(BlueprintCallable)
		virtual void LoadFromData(const TArray<uint8>& FileData) = 0;
};

/**
 * 
 */
UCLASS(NotBlueprintable,NotBlueprintType)
class MUTHM_API UMMScriptImpl : public UObject,public IMMScript
{
	GENERATED_BODY()
	void _DeserializeInternal(char* _JsonStr);
public:
	virtual void LoadFromFile(FString FileName) override;
	virtual void LoadFromData(const TArray<uint8>& FileData) override;
};
