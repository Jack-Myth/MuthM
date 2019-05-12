// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ModManager.generated.h"

USTRUCT(BlueprintType)
struct FModInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		int32 ModID;
	UPROPERTY(BlueprintReadOnly)
		FString ModName;
	UPROPERTY(BlueprintReadOnly)
		FString MountPoint;
	UPROPERTY(BlueprintReadOnly)
		FString ModInfoFile;
	UPROPERTY(BlueprintReadOnly)
		FString ModFileName;
	UPROPERTY(BlueprintReadOnly)
		int VersionCode;
	UPROPERTY(BlueprintReadOnly)
		FString Version;
	UPROPERTY(BlueprintReadOnly)
		int ModSize;
	UPROPERTY(BlueprintReadOnly)
		FText ModDescription;
	UPROPERTY(BlueprintReadOnly)
		bool bIsEnabled;
};

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

	static TScriptInterface<IModManager> Get(const UObject* WorldContextObj);
	virtual void InitLoadMods() = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool LoadMod(int ModID) = 0;
	UFUNCTION(BlueprintCallable)
		virtual TArray<FModInfo> GetModsList() const = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool GetModInfoByID(int ModID,FModInfo& Result) const = 0;
	UFUNCTION(BlueprintCallable)
		virtual void UnloadMod(int ModToUnload) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool IsModLoaded(int ModID) const = 0;
	UFUNCTION(BlueprintCallable)
		virtual void SetModEnabled(int ModID, bool Enabled) = 0;
};
