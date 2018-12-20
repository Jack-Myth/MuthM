// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MIMEHelper.h"
#include "Paths.h"
#include "FileHelper.h"

TMap<FString, FString> MIMEHelper::MIMERecords;// = MIMEHelper::InitMIME();
TMap<FString, FString> MIMEHelper::InitMIME()
{
	TMap<FString, FString> tMIMERecords;
	TArray<FString> MIMERecText;
	FFileHelper::LoadFileToStringArray(MIMERecText, *FPaths::Combine(FPaths::ProjectContentDir(), "/Non-Assets/MIME.txt"));
	for (int i=0;i<MIMERecText.Num();i++)
	{
		FString L, R;
		MIMERecText[i].Split(" ", &L, &R);
		tMIMERecords.Add(L)=R;
	}
	return tMIMERecords;
}

FString MIMEHelper::ExtensionToMIMEType(FString Extension)
{
	if (Extension[0] == TEXT('.'))
		Extension = Extension.Mid(1);
	Extension.ToLowerInline();
	FString* MIMEType;
	MIMEType = MIMERecords.Find(Extension);
	if (MIMEType)
		return *MIMEType;
	else
		return MIMERecords["*"];
}

FString MIMEHelper::MIMETypeToExtension(FString MIMEType)
{
	MIMEType.ToLowerInline();
	for (auto it=MIMERecords.CreateIterator();it;++it)
	{
		if (it->Value == MIMEType)
			return it->Key;
	}
	return "*";
}
