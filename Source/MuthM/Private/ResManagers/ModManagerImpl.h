// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ModManager.h"
#include "Object.h"
#include "ModManagerImpl.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMModManager, Log, All)

/**
 * 
 */
UCLASS()
class MUTHM_API UModManagerImpl : public UObject,public IModManager
{
	GENERATED_BODY()
	class FPakPlatformFile* _ModPlatformFile=nullptr; //Native Ptr, hold ref and prevent to be recycled.
	TArray<FModInfo> ModsList;
	TArray<class UModInstanceBase*> ModInstanceList;
	void RefreshModsList();
	bool LoadModInternal(FModInfo& ModInfo);
public:
	virtual void InitLoadMods() override;
	virtual TArray<FModInfo> GetModsList() const override;
	virtual bool LoadMod(int ModToLoad) override;
	virtual void UnloadMod(int ModToUnload) override;
	virtual bool IsModLoaded(int ModID) const override;
	virtual void SetModEnabled(int ModID, bool Enabled) override;


	virtual bool GetModInfoByID(int ModID,FModInfo& Result) const override;

};
