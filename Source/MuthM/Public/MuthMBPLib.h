// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InstructionManager.h"
#include "MuthMBPLib.generated.h"

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
};
