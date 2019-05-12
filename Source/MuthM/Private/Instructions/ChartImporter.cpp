// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ChartImporter.h"
#include "ChartImporterCytus.h"
#include "NoExportTypes.h"
#include "Engine/World.h"

// Add default functionality here for any IChartImporter functions that are not pure virtual.

TScriptInterface<IChartImporter> IChartImporter::Get(UObject* WorldContextObj,EChartImporterType ImporterType)
{
	switch (ImporterType)
	{
		case CIT_Cytus:
			return NewObject<UChartImporterCytus>(WorldContextObj->GetWorld());
			break;
		default:
			break;
	}
	return nullptr;
}