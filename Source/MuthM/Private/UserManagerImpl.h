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
	
public:
	virtual void Login() override;


	virtual void Logout() override;

	virtual void IsLoggedIn() const override;

	virtual int GetUserID() const override;


	virtual bool QueryPlayerAccount(FUserInfo& UserInfo) override;


	virtual void QueryAccountByID(int UserID, FUserInfo& UserInfo) override;


	virtual bool UploadPlayerAvatar() override;


	virtual bool UploadMDAT(const TArray<uint8>& MDATData) override;


	virtual bool UploadMusic(const TArray<uint8>& OpusData) override;


	virtual FString GetAvatarURL(int UserID) override;


};
