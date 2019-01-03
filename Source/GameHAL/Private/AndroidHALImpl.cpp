// Copyright (C) 2018 JackMyth. All Rights Reserved.

#if PLATFORM_ANDROID
#include "AndroidHALImpl.h"
#include "MIMEHelper.h"
#include "PlatformProcess.h"
#include "Async.h"

void AndroidHALImpl::_OnOpenFileResult(JNIEnv * jenv, jobject jobj1, jobject jobj2, jint requestCode, jint resultCode, jobject data)
{
	//First Remove Delegate
	FJavaWrapper::OnActivityResultDelegate.Remove(OpenDelegateHandle);
	if (requestCode != 12888)
		return;
	int RESULT_CANCELED = 0;
	int RESULT_OK = -1;
	if (resultCode == RESULT_OK)
	{
		//JNI get Uri from Intent
		jmethodID getPathID = FJavaWrapper::FindMethod(jenv, FJavaWrapper::GameActivityClassID, "getPath", "(Landroid/net/Uri;)Ljava/lang/String;", false);
		jclass IntentID = FJavaWrapper::FindClass(jenv, "android/content/Intent", false);
		jmethodID getDataID = FJavaWrapper::FindMethod(jenv, IntentID, "getData", "()Landroid/net/Uri;", false);
		jobject uriObj = FJavaWrapper::CallObjectMethod(jenv, data, getDataID);
		//Get real path of the Uri.
		jstring OpenFileNamejStr = (jstring)FJavaWrapper::CallObjectMethod(jenv, FJavaWrapper::GameActivityThis, getPathID, uriObj);
		const char* UTFStr = jenv->GetStringUTFChars(OpenFileNamejStr, NULL);
		FString OpenedFileName = UTF8_TO_TCHAR(UTFStr);
		jenv->ReleaseStringUTFChars(OpenFileNamejStr, UTFStr);
		AsyncTask(ENamedThreads::GameThread, [=]()
			{
				TArray<FString> Result;
				Result.Add(OpenedFileName);
				LastOpenFileDelegate.ExecuteIfBound(true, Result);
				LastOpenFileDelegate.Unbind();
			});
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [=]()
			{
				LastOpenFileDelegate.ExecuteIfBound(false, TArray<FString>());
				LastOpenFileDelegate.Unbind();
			});
	}
}

void AndroidHALImpl::OpenFileDialog(const FString& Title, const FString& DefaultPath, const TArray<FString>& Filters, bool AllowMultipleSelected, FOnGetOpenFileName Delegate)
{
	if (LastOpenFileDelegate.IsBound())
		Delegate.ExecuteIfBound(false, TArray<FString>());
	LastOpenFileDelegate = Delegate;
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
	FJavaWrapper::CallVoidMethod(java_env, FJavaWrapper::GameActivityThis, GetOpenFileNameID,MIMEJavaString);
	OpenDelegateHandle = FJavaWrapper::OnActivityResultDelegate.AddRaw(this, &AndroidHALImpl::_OnOpenFileResult);
}
#endif
