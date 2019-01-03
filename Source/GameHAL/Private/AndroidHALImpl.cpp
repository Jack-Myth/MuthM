// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "AndroidHALImpl.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include "MIMEHelper.h"
#include "PlatformProcess.h"
#include "Async.h"

void AndroidHALImpl::OpenFileDialog(const FString& Title, const FString& DefaultPath, const TArray<FString>& Filters, bool AllowMultipleSelected, FOnGetOpenFileName Delegate)
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
	FJavaWrapper::CallVoidMethod(java_env, FJavaWrapper::GameActivityThis, GetOpenFileNameID,MIMEJavaString);
	FDelegateHandle* LambdaHandle=new FDelegateHandle();
	JavaVM * vm;
	java_env->GetJavaVM(&vm);
	*LambdaHandle = FJavaWrapper::OnActivityResultDelegate.AddLambda([=](JNIEnv * jenv, jobject jobj1, jobject jobj2, jint requestCode, jint resultCode, jobject data)
		{
			UE_LOG(LogTemp, Log, TEXT("Enter OnActivityResultDelegate"));
			//First Remove Delegate
			FJavaWrapper::OnActivityResultDelegate.Remove(*LambdaHandle);
			UE_LOG(LogTemp, Log, TEXT("Remove Lambda from OnActivityResultDelegate"));
			if (requestCode != 12888)
				return;
			UE_LOG(LogTemp, Log, TEXT("getPathID"));
			int RESULT_CANCELED = 0;
			int RESULT_OK = -1;
			if (resultCode == RESULT_OK)
			{
				jmethodID getPathID = FJavaWrapper::FindMethod(jenv, FJavaWrapper::GameActivityClassID, "getPath", "(Landroid/net/Uri;)Ljava/lang/String;", false);
				UE_LOG(LogTemp, Log, TEXT("OpenFileNamejStr"));
				jclass IntentID = FJavaWrapper::FindClass(jenv, "android/content/Intent", false);
				jmethodID getDataID = FJavaWrapper::FindMethod(jenv, IntentID, "getData", "()Landroid/net/Uri;", false);
				jobject uriObj = FJavaWrapper::CallObjectMethod(jenv, data, getDataID);
				jstring OpenFileNamejStr = (jstring)FJavaWrapper::CallObjectMethod(jenv, FJavaWrapper::GameActivityThis, getPathID, uriObj);
				UE_LOG(LogTemp, Log, TEXT("UTFStr"));
				const char* UTFStr = jenv->GetStringUTFChars(OpenFileNamejStr, NULL);
				UE_LOG(LogTemp, Log, TEXT("OpenedFileName"));
				FString OpenedFileName = UTF8_TO_TCHAR(UTFStr);
				UE_LOG(LogTemp, Log, TEXT("ReleaseStringUTFChars"));
				jenv->ReleaseStringUTFChars(OpenFileNamejStr, UTFStr);
				UE_LOG(LogTemp, Log, TEXT("Ready to call delegate"));
				AsyncTask(ENamedThreads::GameThread, [=]() 
					{
						UE_LOG(LogTemp, Log, TEXT("Enter Game Thread"));
						TArray<FString> Result;
						UE_LOG(LogTemp, Log, TEXT("Enter Game Thread+1"));
						Result.Add(OpenedFileName);
						UE_LOG(LogTemp, Log, TEXT("Enter Game Thread+2"));
						delete LambdaHandle;
						UE_LOG(LogTemp, Log, TEXT("Enter Game Thread+3"));
						Delegate.ExecuteIfBound(true, Result); 
						UE_LOG(LogTemp, Log, TEXT("Enter Game Thread+4"));
					});
			}
			else
			{
				Delegate.ExecuteIfBound(false, TArray<FString>());
			}
		});
}
#endif
