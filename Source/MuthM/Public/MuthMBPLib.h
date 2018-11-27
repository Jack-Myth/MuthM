// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InstructionManager.h"
#include "DetailStructures.h"
#include "MuthMBPLib.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMBPLib, Log, All);

/**
 * 
 */
UCLASS()
class MUTHM_API UMuthMBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
		static TScriptInterface<IInstructionManager> GetInstructionManager();

	UFUNCTION(BlueprintPure,meta=(WorldContext = "WorldContextObj",ToolTip="Load UTexture2D from the MDAT file that is using in the game."))
		static class UTexture2D* LoadTextureInGame(UObject* WorldContextObj,FString FileName);

	UFUNCTION(BlueprintCallable)
		static class UTexture2D* GetLocalTexture(const FString &_TexPath);
	static class UTexture2D* GetLocalTextureByImageData(const TArray<uint8>& ImageData);
	static TArray<FColor> uint8ToFColor(const TArray<uint8> origin);
	static UTexture2D* TextureFromImage(const int32 SrcWidth, const int32 SrcHeight, const TArray<FColor>&SrcData, const bool UseAlpha);
	UFUNCTION(BlueprintCallable)
		static class USoundWave* DecodeWaveFromOpus(const TArray<uint8>& OpusData);
	UFUNCTION(BlueprintCallable)
		static class UVisualizableSoundWave* DecodeVisualizableWaveFromOpus(const TArray<uint8>& OpusData);
	UFUNCTION(BlueprintCallable)
		static bool ConvertMP3ToOpus(const TArray<uint8>& MP3File,TArray<uint8>& OpusOutput);
	UFUNCTION(BlueprintCallable)
		static void AddStringItemToCategory(UPARAM(ref) FDetailCategoryStruct& DetailCategory, FDetailItemString StringItem);
	UFUNCTION(BlueprintCallable)
		static void AddNumberItemToCategory(UPARAM(ref) FDetailCategoryStruct& DetailCategory, FDetailItemNumber NumberItem);
	UFUNCTION(BlueprintCallable)
		static void AddCustomItemToCategory(UPARAM(ref) FDetailCategoryStruct& DetailCategory, FDetailItemCustom CustomItem);
};
