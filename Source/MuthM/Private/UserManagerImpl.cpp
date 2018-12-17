// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "UserManagerImpl.h"

void UUserManagerImpl::Login()
{

}

void UUserManagerImpl::Logout()
{

}

void UUserManagerImpl::IsLoggedIn() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

int UUserManagerImpl::GetUserID() const
{
	return 0;
}

bool UUserManagerImpl::QueryPlayerAccount(FUserInfo& UserInfo)
{
	return false;
}

void UUserManagerImpl::QueryAccountByID(int UserID, FUserInfo& UserInfo)
{
	//UNDONE:
}

bool UUserManagerImpl::UploadPlayerAvatar()
{
	//UNDONE:
	return false;
}

bool UUserManagerImpl::UploadMDAT(const TArray<uint8>& MDATData)
{
	//UNDONE:
	return false;
}

bool UUserManagerImpl::UploadMusic(const TArray<uint8>& OpusData)
{
	//UNDONE:
	return false;
}

FString UUserManagerImpl::GetAvatarURL(int UserID)
{
	//UNDONE:
	return "";
}