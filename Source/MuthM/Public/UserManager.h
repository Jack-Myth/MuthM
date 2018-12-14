// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UserManager.generated.h"

USTRUCT(BlueprintType)
struct FUserInfo
{
	GENERATED_BODY()
		//Save User Info,Some item may not valid when user not logged in.

	UPROPERTY(BlueprintReadWrite)
		int UserID;
	UPROPERTY(BlueprintReadWrite)
		FString UserName;
	UPROPERTY(BlueprintReadWrite)
		FString LastLoginDate;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI,meta=(CannotImplementInterfaceInBlueprint))
class UUserManager : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class MUTHM_API IUserManager
{
	GENERATED_BODY()

public:
	static TScriptInterface<IUserManager> Get();
	UFUNCTION(BlueprintCallable)
		virtual void Login() = 0;
	UFUNCTION(BlueprintCallable)
		virtual void Logout() = 0;
	UFUNCTION(BlueprintCallable)
		virtual int GetUserID() const = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool QueryPlayerAccount(FUserInfo& UserInfo) = 0;
	UFUNCTION(BlueprintCallable)
		virtual void QueryAccountByID(int UserID, FUserInfo& UserInfo) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool UploadPlayerAvatar() = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool UploadMDAT(const TArray<uint8>& MDATData) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool UploadMusic(const TArray<uint8>& OpusData) = 0;
	UFUNCTION(BlueprintCallable)
		virtual FString GetAvatarURL(int UserID) = 0;
	
};
