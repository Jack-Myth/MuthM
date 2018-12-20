// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "AndroidHALImpl.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"

bool AndroidHALImpl::OpenFileDialog(const FString& Title, const FString& DefaultPath, const TArray<FString>& Filters, bool AllowMultipleSelected, TArray<FString> OpenFileName)
{
	JNIEnv* JE = FAndroidApplication::GetJavaEnv();
	//IDesktopPlatform
}
#endif
