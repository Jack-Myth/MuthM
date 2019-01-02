// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "AndroidHALImpl.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include "MIMEHelper.h"
#include "PlatformProcess.h"

bool AndroidHALImpl::OpenFileDialog(const FString& Title, const FString& DefaultPath, const TArray<FString>& Filters, bool AllowMultipleSelected, TArray<FString>& OpenFileName)
{
	//AllowMultipleSelected is not support yet.
	JNIEnv* java_env = FAndroidApplication::GetJavaEnv();
	jmethodID GetOpenFileNameID = FJavaWrapper::FindMethod(java_env, FJavaWrapper::GameActivityClassID, "GetOpenFileName", "(Ljava/lang/String;)V", false);
	FString MIMEType;
	if (Filters.Num())
	{
		if (Filters[0].Contains("/"))
		{
			//Thread Filters[0] as MIME Type
			MIMEType = Filters[0];
		}
		else
		{
			MIMEType = MIMEHelper::ExtensionToMIMEType(Filters[0]);
		}
	}
	else
	{
		MIMEType = MIMEHelper::ExtensionToMIMEType("*");
	}
	jstring MIMEJavaString = java_env->NewStringUTF(TCHAR_TO_UTF8(*MIMEType));
	FJavaWrapper::CallObjectMethod(java_env, FJavaWrapper::GameActivityThis, GetOpenFileNameID,MIMEJavaString);
	jfieldID OpenFileNameId= FJavaWrapper::FindField(java_env, FJavaWrapper::GameActivityClassID, "OpenFileName", "Ljava/lang/String;", false);
	jstring OpenFileNamejStr = (jstring)java_env->GetObjectField(FJavaWrapper::GameActivityThis, OpenFileNameId);
	FString OpenedFileName;
	while (true)
	{
		const char* UTFStr = java_env->GetStringUTFChars(OpenFileNamejStr, NULL);
		OpenedFileName = UTF8_TO_TCHAR(UTFStr);
		java_env->ReleaseStringUTFChars( OpenFileNamejStr, UTFStr);
		if (OpenedFileName !="")
			break;
		FPlatformProcess::Sleep(0.1f);
	}
	if (OpenedFileName=="*Cancelled*")
	{
		OpenFileName.Empty();
		return false;
	}
	else
	{
		OpenFileName.Empty();
		OpenFileName.Add(OpenedFileName);
		return true;
	}
}
#endif
