// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ScriptInterface.h"
#include "ChartImporter.generated.h"

UENUM(BlueprintType)
enum EChartImporterType
{
	CIT_Cytus UMETA(DisplayName="Cytus")
};

UINTERFACE(MinimalAPI)
class UChartImporter : public UInterface
{
	GENERATED_BODY()
};

/**
 * Import Score from other game's chart.
 */
class MUTHM_API IChartImporter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
		TScriptInterface<IChartImporter> GetChartImporter(EChartImporterType ImporterType);
	virtual bool ImportFromFile(FString FilePath, TArray<class UInstruction*>& ImportResult) = 0;
};
