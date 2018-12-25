// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UserManager.h"
#include "UserManagerImpl.generated.h"

/**
 * 
 */
UCLASS()
class UUserManagerImpl : public UObject,public IUserManager
{
	GENERATED_BODY()
		//;
	UPROPERTY()
		UUserManagerDelegates* UserManagerDelegates;
	UPROPERTY()
		int _UserID;
	UPROPERTY()
		FString _Token;
public:
	UUserManagerImpl();

	virtual void Login(FString LoginName, FString Passworld) override;


	virtual void LoginByToken(int UserID, FString Token) override;


	virtual void Logout() override;


	virtual bool IsLoggedIn() const override;


	virtual int GetUserID() const override;


	virtual bool QueryPlayerAccount(FOnUserQueryResult SelfQueryResult) override;


	virtual void QueryAccountByID(int UserID, FOnUserQueryResult UserQueryResult) override;


	virtual FString GetAvatarURL(int UserID) const override;


	virtual UUserManagerDelegates* GetUserDelegate() const override;


	virtual bool AddAvatarUploadTask(const FString& AvatarFileName) override;


	virtual FString GenCookies() const override;


	virtual bool UploadMDATLinkOnly(const FString& MDATURL, const FString& Title, const FString& Description, int MDATID = 0) override;


	virtual bool AddMDATUploadTask(const FString& MDATFileName, const FString& Title, const FString& Description, int MDATID = 0) override;

};
