// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class GAMEHAL_API MIMEHelper
{
	static TMap<FString, FString> MIMERecords;
	static TMap<FString, FString> InitMIME();
public:
	//Find MIMEType for Extension,return  if extension can't be found.
	static FString ExtensionToMIMEType(FString Extension);
	static FString MIMETypeToExtension(FString MIMEType);
};
