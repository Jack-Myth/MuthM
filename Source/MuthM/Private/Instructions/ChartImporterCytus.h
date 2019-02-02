// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ChartImporter.h"
#include "ChartImporterCytus.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ChartImporterCytus, Log, All);

/**
 * 
 */
UCLASS()
class UChartImporterCytus : public UObject,public IChartImporter
{
	GENERATED_BODY()
	
public:
	virtual bool ImportFromFile(FString FilePath, TArray<class UInstruction *>& ImportResult) override;

};
