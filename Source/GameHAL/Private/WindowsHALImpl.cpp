// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "WindowsHALImpl.h"
#include "GameHAL.h"

#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#include "commdlg.h"
#include "MIMEHelper.h"

void WindowsHALImpl::OpenFileDialog(const FString& Title, const FString& DefaultPath, const TArray<FString>& Filters, bool AllowMultipleSelected, FOnGetOpenFileName Delegate)
{
	OPENFILENAME ofn = { NULL };
	TArray<TCHAR> FileName;
	FileName.SetNum(AllowMultipleSelected ? 20480 : 512); //Buffer Should be longer when select multiple files.
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = FileName.GetData();
	ofn.nMaxFile = FileName.Num();
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = *Title;
	ofn.lpstrInitialDir = *DefaultPath;
	if (AllowMultipleSelected)
		ofn.Flags = ofn.Flags | OFN_ALLOWMULTISELECT;
	TArray<TCHAR> FiltersArray;
	TArray<FString> TargetFilters = Filters;
	//TODO: a little performance issue.
	if (TargetFilters[0].Contains("/"))
		TargetFilters[0] = MIMEHelper::MIMETypeToExtension(TargetFilters[0]);
	for (int i = 0; i < TargetFilters.Num(); i++)
	{
		FiltersArray.Append(*TargetFilters[i], TargetFilters[i].Len());
		FiltersArray.Append(TEXT(" File"), 6); //Include Terminal Character.
		FiltersArray.Append(TEXT("*."), 2);
		FiltersArray.Append(*TargetFilters[i], TargetFilters[i].Len() + 1); //Include Terminal Character.
	}
	FiltersArray.Add(0);
	ofn.lpstrFilter = FiltersArray.GetData();
	if (GetOpenFileName(&ofn))
	{
		TArray<FString> OpenFileName;
		int FileNameIndex = 0;
		while (FileName[FileNameIndex])
		{
			FString FileNameStr = FileName.GetData() + FileNameIndex;
			FileNameIndex += FileNameStr.Len() + 1;
			OpenFileName.Add(FileNameStr);
		}
		Delegate.ExecuteIfBound(true, OpenFileName);
	}
	else
	{
		//User Cancelled.
		Delegate.ExecuteIfBound(false, TArray<FString>());
	}
}

#include "HideWindowsPlatformTypes.h"

#endif