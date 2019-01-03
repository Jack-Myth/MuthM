// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGameHAL.h"
#include "IDelegateInstance.h"

/**
 * 
 */

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"

class AndroidHALImpl:public IGameHAL
{
	FOnGetOpenFileName LastOpenFileDelegate;
	FDelegateHandle OpenDelegateHandle;
	void _OnOpenFileResult(JNIEnv * jenv, jobject jobj1, jobject jobj2, jint requestCode, jint resultCode, jobject data);
public:
	virtual void OpenFileDialog(const FString& Title, const FString& DefaultPath, const TArray<FString>& Filters, bool AllowMultipleSelected, FOnGetOpenFileName Delegate) override;

};

typedef AndroidHALImpl GameHALImpl;

#endif