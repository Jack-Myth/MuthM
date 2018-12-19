// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "WindowsHALImpl.h"
#include "GameHAL.h"

#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#include "commdlg.h"

bool WindowsHALImpl::OpenFileDialog(const FString& Title, const FString& DefaultPath, const TArray<FString>& Filters, bool AllowMultipleSelected, TArray<FString> OpenFileName)
{
	OPENFILENAME ofn = {NULL};
	TArray<TCHAR> FileName;
	FileName.SetNum(AllowMultipleSelected?20480:512); //Buffer Should be longer when select multiple files.
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = FileName.GetData();
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = *Title;
	ofn.lpstrInitialDir = *DefaultPath;
	TArray<TCHAR> FiltersArray;
	for (int i=0;i<Filters.Num();i++)
		FiltersArray.Append(*Filters[i], Filters[i].Len() + 1);
	FiltersArray.Add(0);
	ofn.lpstrFilter = FiltersArray.GetData();
	if (GetOpenFileName(&ofn))
	{
		OpenFileName.Empty();
		int FileNameIndex = 0;
		while (FileName[FileNameIndex])
		{
			FString FileNameStr = FileName.GetData() + FileNameIndex;
			FileNameIndex += FileNameStr.Len() + 1;
			OpenFileName.Add(FileNameStr);
		}
		return true;
	}
	return false;
}
#include "HideWindowsPlatformTypes.h"

#endif