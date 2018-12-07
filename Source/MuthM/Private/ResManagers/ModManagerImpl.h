// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ModManager.h"
#include "Object.h"
#include "ModManagerImpl.generated.h"

/**
 * 
 */
UCLASS()
class MUTHM_API UModManagerImpl : public UObject,public IModManager
{
	GENERATED_BODY()
	class FPakPlatformFile* _ModPlatformFile=nullptr;
public:
	virtual void InitLoadMods() override;
	virtual bool LoadModFromPath(FString ModFileName) override;
	virtual void UnloadModByName(FName ModName) override;

};
