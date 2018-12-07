// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ModManager.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UModManager : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MUTHM_API IModManager
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	static TScriptInterface<IModManager> Get();
	virtual void InitLoadMods() = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool LoadModFromPath(FString ModFileName) = 0;
	virtual void UnloadModByName(FName ModName) = 0;
};
