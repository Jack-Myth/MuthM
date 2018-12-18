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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserLoginEvent, int, UserID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserLoginFailed, FString,UserLoginName,FText,ErrorMsg);

UCLASS(MinimalAPI)
class UUserManagerDelegates:public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FOnUserLoginEvent OnUserLoggedIn;
	UPROPERTY(BlueprintAssignable)
		FOnUserLoginEvent OnUserLoggedOut;
	UPROPERTY(BlueprintAssignable)
		FOnUserLoginFailed OnUserLoginFailed;
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
	static TScriptInterface<IUserManager> Get(const UObject* WorldContextObj);
	UFUNCTION()
		virtual void Login(FString LoginName,FString Passworld) = 0;
	UFUNCTION()
		virtual void LoginByToken(int UserID, FString Token) = 0;
	UFUNCTION()
		virtual void Logout() = 0;
	UFUNCTION(BlueprintCallable)
		virtual bool IsLoggedIn() const = 0;
	UFUNCTION(BlueprintCallable)
		virtual int GetUserID() const = 0;
	UFUNCTION()
		virtual bool QueryPlayerAccount(FOnUserQueryResult SelfQueryResult) = 0;
	UFUNCTION(BlueprintCallable)
		virtual void QueryAccountByID(int UserID, FOnUserQueryResult UserQueryResult) = 0;
	UFUNCTION()
		virtual bool AddAvatarUploadTask(const FString& AvatarFileName) = 0;
	UFUNCTION()
		virtual bool UploadMDATLinkOnly(const FString& MDATURL, const FString& Title, const FString& Description, int MDATID = 0) = 0;
	UFUNCTION()
		virtual bool AddMDATUploadTask(const FString& MDATFileName, const FString& Title, const FString& Description, int MDATID = 0) = 0;
	UFUNCTION()
		virtual bool UploadMusicLinkOnly(const FString& MusicURL, const FString& Title, const FString& Musician, int MusicID = 0) = 0;
	UFUNCTION()
		virtual bool AddMusicUploadTask(const FString& OpusFileName, const FString& Title, const FString& Musician, int MusicID = 0) = 0;
	UFUNCTION(BlueprintCallable)
		virtual FString GetAvatarURL(int UserID) const = 0;
	UFUNCTION(BlueprintCallable)
		virtual UUserManagerDelegates* GetUserDelegate() const = 0;
	virtual FString GenCookies() const = 0;
		
};
