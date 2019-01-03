// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MIMEHelper.h"
#include "Paths.h"
#include "FileHelper.h"

FString MIMEHelper::ExtensionToMIMEType(FString Extension)
{
	if (Extension[0] == TEXT('.'))
		Extension = Extension.Mid(1);
	Extension.ToLowerInline();
	TArray<FString> MIMERecText;
	FFileHelper::LoadFileToStringArray(MIMERecText, TEXT("/Game/MuthM/Non-Assets/MIME.txt"));
	for (int i = 0; i < MIMERecText.Num(); i++)
	{
		FString L, R;
		MIMERecText[i].Split(" ", &L, &R);
		if (L == Extension)
			return R;
	}
	return "*/*";
}

FString MIMEHelper::MIMETypeToExtension(FString MIMEType)
{
	MIMEType.ToLowerInline();
	TArray<FString> MIMERecText;
	FFileHelper::LoadFileToStringArray(MIMERecText, TEXT("/Game/MuthM/Non-Assets/MIME.txt"));
	for (int i = 0; i < MIMERecText.Num(); i++)
	{
		FString L, R;
		MIMERecText[i].Split(" ", &L, &R);
		if (R == MIMEType)
			return L;
	}
	return "*";
}
