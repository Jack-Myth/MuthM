// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "UserManagerImpl.h"
#include "HttpModule.h"
#include "SecureHash.h"
#include "NetworkManager.h"
#include "IHttpResponse.h"
#include "MuthMBPLib.h"
#include "JsonObject.h"
#include "Regex.h"
#include "DownloadManager.h"
#include "GenericPlatformHttp.h"

#define LOCTEXT_NAMESPACE "MuthM"

UUserManagerImpl::UUserManagerImpl()
{
	UserManagerDelegates = CreateDefaultSubobject<UUserManagerDelegates>("UserManagerDelegates");
}

void UUserManagerImpl::Login(FString LoginName, FString Passworld)
{
	if (IsLoggedIn())
		return;
	TSharedPtr<IHttpRequest> LoginRequest = FHttpModule::Get().CreateRequest();
	FString PostData = "username=" + LoginName + "&passmd5=" + FMD5::HashAnsiString(*Passworld);
	LoginRequest->SetURL(FString(MUTHM_URL_ROOT) + "/login.php");
	LoginRequest->SetVerb("POST");
	LoginRequest->SetContentAsString(PostData);
	LoginRequest->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (bConnectedSuccessfully)
			{
				TSharedPtr<FJsonObject> JsonObj = UMuthMBPLib::DeserializeJsonFromStr(Response->GetContentAsString());
				int32 errCode;
				if (!JsonObj->TryGetNumberField("ErrCode",errCode)||errCode)
				{
					UserManagerDelegates->OnUserLoginFailed.Broadcast(LoginName, LOCTEXT("Please verify userName and password", "Please verify userName and password."));
					return;
				}
				FString Cookies = Response->GetHeader("Set-Cookie");
				FRegexPattern UserIDRegex("UserID=[0-9]*");
				FRegexPattern TokenRegex("Token=[0-9,a-z,A-Z]*");
				FRegexMatcher UserIDMatcher(UserIDRegex, Cookies);
				FRegexMatcher TokenMatcher(TokenRegex, Cookies);
				FString UserIDStr;
				FString TokenStr;
				if (UserIDMatcher.FindNext())
					UserIDStr = Cookies.Mid(UserIDMatcher.GetMatchBeginning(), UserIDMatcher.GetMatchEnding());
				if (TokenMatcher.FindNext())
					TokenStr = Cookies.Mid(TokenMatcher.GetMatchBeginning(), TokenMatcher.GetMatchEnding());
				_UserID = FCString::Atoi(*UserIDStr);
				_Token = TokenStr;
				UserManagerDelegates->OnUserLoggedIn.Broadcast(_UserID);
			}
			else
			{
				//Login Failed.
				UserManagerDelegates->OnUserLoginFailed.Broadcast(LoginName, LOCTEXT("Failed to connect to server", "Failed to connect to server."));
			}
		});
	LoginRequest->ProcessRequest();
}

void UUserManagerImpl::Logout()
{
	_UserID = -1;
	_Token = "";
	//UNDONE: Logout();
	//throw std::logic_error("The method or operation is not implemented.");
}

bool UUserManagerImpl::IsLoggedIn() const
{
	return _UserID > 0;
}

int UUserManagerImpl::GetUserID() const
{
	return _UserID;
}

bool UUserManagerImpl::QueryPlayerAccount(FOnUserQueryResult SelfQueryResult)
{
	if (!IsLoggedIn())
		return false;
	TSharedPtr<IHttpRequest> Request = INetworkManager::Get(this)->GenRequest();
	Request->SetURL(FString::Printf(TEXT("%s/query_user_byid.php?UserID=%d"), TEXT(MUTHM_URL_ROOT), _UserID));
	Request->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			FUserInfo ResultUsrInfo;
			if (!bConnectedSuccessfully)
			{
				SelfQueryResult.ExecuteIfBound(_UserID, false, ResultUsrInfo);
				return;
			}
			TSharedPtr<FJsonObject> JsonObj = UMuthMBPLib::DeserializeJsonFromStr(Response->GetContentAsString());
			int ErrCode;
			if (!JsonObj->TryGetNumberField("ErrCode", ErrCode) || ErrCode)
			{
				//Err when query.
				SelfQueryResult.ExecuteIfBound(_UserID, false, ResultUsrInfo);
				return;
			}
			ResultUsrInfo.LastLoginDate = JsonObj->GetStringField("LastLoginDate");
			ResultUsrInfo.UserID = JsonObj->GetIntegerField("ID");
			ResultUsrInfo.UserName = JsonObj->GetStringField("UserName");
			SelfQueryResult.ExecuteIfBound(_UserID, true, ResultUsrInfo);
		});
	return Request->ProcessRequest();
}

void UUserManagerImpl::QueryAccountByID(int UserID, FOnUserQueryResult UserQueryResult)
{
	TSharedPtr<IHttpRequest> Request = INetworkManager::Get(this)->GenRequest();
	Request->SetURL(FString::Printf(TEXT("%s/query_user_byid.php?UserID=%d"), TEXT(MUTHM_URL_ROOT), UserID));
	Request->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			FUserInfo ResultUsrInfo;
			if (!bConnectedSuccessfully)
			{
				UserQueryResult.ExecuteIfBound(UserID, false, ResultUsrInfo);
				return;
			}
			TSharedPtr<FJsonObject> JsonObj = UMuthMBPLib::DeserializeJsonFromStr(Response->GetContentAsString());
			int ErrCode;
			if (!JsonObj->TryGetNumberField("ErrCode", ErrCode) || ErrCode)
			{
				//Err when query.
				UserQueryResult.ExecuteIfBound(UserID, false, ResultUsrInfo);
				return;
			}
			ResultUsrInfo.UserID = JsonObj->GetIntegerField("ID");
			ResultUsrInfo.UserName = JsonObj->GetStringField("UserName");
			UserQueryResult.ExecuteIfBound(UserID, true, ResultUsrInfo);
		});
}

bool UUserManagerImpl::AddAvatarUploadTask(const FString& AvatarFileName)
{
	if (!IsLoggedIn())
		return false;
	return !!IDownloadManager::Get(this)->SubmitUploadTask(AvatarFileName, FString(MUTHM_URL_ROOT) + "/upload_avatar.php","Avatar","Avatar");
}

bool UUserManagerImpl::AddMDATUploadTask(const FString& MDATFileName, const FString& Title, const FString& Description, int MDATID /*= 0*/)
{
	if (!IsLoggedIn())
		return false;
	TSharedPtr<IHttpRequest> Request = INetworkManager::Get(this)->GenRequest();
	FString EncodedTitle = FGenericPlatformHttp::UrlEncode(Title);
	FString EncodedDescription = FGenericPlatformHttp::UrlEncode(Description);
	FString URL = FString::Printf(TEXT("%s/upload_mdat.php?Title=%s&LinkOnly=false&Description=%s"), *EncodedTitle, *EncodedDescription);
	return !!IDownloadManager::Get(this)->SubmitUploadTask(MDATFileName, URL, "MDATFile");
}

bool UUserManagerImpl::UploadMusicLinkOnly(const FString& MusicURL, const FString& Title, const FString& Musician, int MusicID /*= 0*/)
{
	if (!IsLoggedIn())
		return false;
	TSharedPtr<IHttpRequest> Request = INetworkManager::Get(this)->GenRequest();
	FString EncodedTitle = FGenericPlatformHttp::UrlEncode(Title);
	FString EncodedMusician = FGenericPlatformHttp::UrlEncode(Musician);
	FString URL = FString::Printf(TEXT("%s/upload_music.php?Title=%s&LinkOnly=true&MusicURL=%s&Description=%s"), *EncodedTitle, *MusicURL, *EncodedMusician);
	Request->SetURL(URL);
	return Request->ProcessRequest();
}

bool UUserManagerImpl::AddMusicUploadTask(const FString& OpusFileName, const FString& Title, const FString& Musician, int MusicID /*= 0*/)
{
	if (!IsLoggedIn())
		return false;
	TSharedPtr<IHttpRequest> Request = INetworkManager::Get(this)->GenRequest();
	FString EncodedTitle = FGenericPlatformHttp::UrlEncode(Title);
	FString EncodedMusician = FGenericPlatformHttp::UrlEncode(Musician);
	FString URL = FString::Printf(TEXT("%s/upload_mdat.php?Title=%s&LinkOnly=false&Description=%s"), *EncodedTitle, *EncodedMusician);
	return !!IDownloadManager::Get(this)->SubmitUploadTask(OpusFileName, URL, "MusicFile");
}

FString UUserManagerImpl::GenCookies() const
{
	return FString::Printf(TEXT("UserID=%d;Token=%s"), _UserID, *_Token);
}

bool UUserManagerImpl::UploadMDATLinkOnly(const FString& MDATURL, const FString& Title, const FString& Description, int MDATID /*= 0*/)
{
	if (!IsLoggedIn())
		return false;
	TSharedPtr<IHttpRequest> Request = INetworkManager::Get(this)->GenRequest();
	FString EncodedTitle = FGenericPlatformHttp::UrlEncode(Title);
	FString EncodedDescription = FGenericPlatformHttp::UrlEncode(Description);
	FString URL = FString::Printf(TEXT("%s/upload_mdat.php?Title=%s&LinkOnly=true&MDATURL=%s&Description=%s"), *EncodedTitle,*MDATURL, *EncodedDescription);
	Request->SetURL(URL);
	return Request->ProcessRequest();
}

FString UUserManagerImpl::GetAvatarURL(int UserID) const
{
	return FString::Printf(TEXT("%s/get_avatar.php?UserID=%d"), TEXT(MUTHM_URL_ROOT), _UserID);
}

UUserManagerDelegates* UUserManagerImpl::GetUserDelegate() const
{
	return UserManagerDelegates;
}

void UUserManagerImpl::LoginByToken(int UserID, FString Token)
{
	if (IsLoggedIn())
		return;
	TSharedPtr<IHttpRequest> LoginRequest = FHttpModule::Get().CreateRequest();
	LoginRequest->SetURL(FString(MUTHM_URL_ROOT) + "/check_login.php");
	LoginRequest->SetVerb("GET");
	LoginRequest->SetHeader("Cookie", GenCookies());
	LoginRequest->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (bConnectedSuccessfully)
			{
				TSharedPtr<FJsonObject> JsonObj = UMuthMBPLib::DeserializeJsonFromStr(Response->GetContentAsString());
				int32 errCode;
				if (!JsonObj->TryGetNumberField("ErrCode", errCode) || errCode)
				{
					UserManagerDelegates->OnUserLoginFailed.Broadcast(FString::FromInt(UserID), LOCTEXT("Login state out of date,please relogin", "Login state out of date,please relogin."));
					return;
				}
				_UserID = UserID;
				_Token = Token;
				UserManagerDelegates->OnUserLoggedIn.Broadcast(_UserID);
			}
			else
			{
				//Login Failed.
				UserManagerDelegates->OnUserLoginFailed.Broadcast(FString::FromInt(UserID), LOCTEXT("Failed to connect to server", "Failed to connect to server."));
			}
		});
	LoginRequest->ProcessRequest();
}

#undef LOCTEXT_NAMESPACE