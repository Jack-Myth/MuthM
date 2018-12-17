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

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnUserQueryResult, int, UserID, bool, IsQuerySuccessful, const FUserInfo&, UserInfo);

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
	static TScriptInterface<IUserManager> Get(const UObject* WorldContextObj);
	UFUNCTION(BlueprintCallable)
		virtual void Login() = 0;
	UFUNCTION(BlueprintCallable)
		virtual void Logout() = 0;
	UFUNCTION(BlueprintCallable)
		virtual void IsLoggedIn() const = 0;
	UFUNCTION(BlueprintCallable)
		virtual int GetUserID() const = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool QueryPlayerAccount(FOnUserQueryResult SelfQueryResult) = 0;
	UFUNCTION(BlueprintCallable)
		virtual void QueryAccountByID(int UserID, FOnUserQueryResult UserQueryResult) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool UploadPlayerAvatar(const TArray<uint8>& AvatarData) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool AddMDATUploadTask(const TArray<uint8>& MDATData) = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool AddMusicUploadTask(const TArray<uint8>& OpusData) = 0;
	UFUNCTION(BlueprintCallable)
		virtual FString GetAvatarURL(int UserID) = 0;
		
};
