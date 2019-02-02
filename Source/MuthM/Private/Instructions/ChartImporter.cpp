// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ChartImporter.h"
#include "ChartImporterCytus.h"

// Add default functionality here for any IChartImporter functions that are not pure virtual.

TScriptInterface<IChartImporter> IChartImporter::GetChartImporter(EChartImporterType ImporterType)
{
	switch (ImporterType)
	{
		case CIT_Cytus:
			return NewObject<UChartImporterCytus>();
			break;
		default:
			break;
	}
}