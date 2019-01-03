// Copyright (C) 2018 JackMyth. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DetailStructures.h"
#include "BlueprintJsonLibrary.h"
#include "MuthMBPLib.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MuthMBPLib, Log, All);

USTRUCT(BlueprintType)
struct FMusicScoreInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
		FString Title;
	UPROPERTY(BlueprintReadWrite)
		FString Subtitle;
	UPROPERTY(BlueprintReadWrite)
		FString Img;
	UPROPERTY(BlueprintReadWrite)
		int MusicID;
	UPROPERTY(BlueprintReadWrite)
		FString ScoreDataFileName;
	UPROPERTY(BlueprintReadWrite)
		TArray<FName> RequestMods;
	UPROPERTY(BlueprintReadWrite)
		TArray<FName> OptionalMods;
};

USTRUCT(BlueprintType)
struct FMDATMainInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString Title;
	UPROPERTY(BlueprintReadWrite)
		int AuthorID;
	UPROPERTY(BlueprintReadWrite)
		FString AuthorName;
	UPROPERTY(BlueprintReadWrite)
		FString Description;
	UPROPERTY(BlueprintReadWrite)
		FString Cover;
	UPROPERTY(BlueprintReadWrite)
		TArray<FMusicScoreInfo> ScoreInfoCollection;
};

/**
 * 
 */
UCLASS()
class MUTHM_API UMuthMBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure,meta=(WorldContext="WorldContextObj"))
		static TScriptInterface<class IInstructionManager> GetInstructionManager(const UObject* WorldContextObj);
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObj"))
		static TScriptInterface<class IModManager> GetModManager(const UObject* WorldContextObj);
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObj"))
		static TScriptInterface<class INetworkManager> GetNetworkManager(const UObject* WorldContextObj);
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObj"))
		static TScriptInterface<class IDownloadManager> GetDownloadManager(const UObject* WorldContextObj);
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObj"))
		static TScriptInterface<class IMusicManager> GetMusicManager(const UObject* WorldContextObj);
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObj"))
		static TScriptInterface<class IUserManager> GetUserManager(const UObject* WorldContextObj);

	UFUNCTION(BlueprintPure,meta=(WorldContext = "WorldContextObj",ToolTip="Load UTexture2D from the MDAT file that is using in the game."))
		static class UTexture2D* LoadTextureInGame(UObject* WorldContextObj,FString FileName);

	UFUNCTION(BlueprintCallable)
		static class UTexture2D* GetLocalTexture(const FString &_TexPath);
	UFUNCTION(BlueprintCallable)
		static class UTexture2D* GetLocalTextureByImageData(const TArray<uint8>& ImageData);
	static TArray<FColor> uint8ToFColor(const TArray<uint8> origin);
	static UTexture2D* TextureFromImage(const int32 SrcWidth, const int32 SrcHeight, const TArray<FColor>&SrcData, const bool UseAlpha);
	UFUNCTION(BlueprintPure)
		static FBlueprintJsonObject LoadJsonFromStr(FString JsonStr);
	static TSharedPtr<FJsonObject> DeserializeJsonFromUTF8(const TArray<uint8>& CharArray);
	static TSharedPtr<FJsonObject> DeserializeJsonFromStr(FString JsonStr);
	UFUNCTION(BlueprintCallable)
		static class UMainSoundWave* DecodeWaveFromOGG(const TArray<uint8>& OGGData);
	UFUNCTION(BlueprintCallable)
		static bool ConvertMP3ToOGG(const TArray<uint8>& MP3File,TArray<uint8>& OGGOutput);
	UFUNCTION(BlueprintCallable)
		static void AddStringItemToCategory(UPARAM(ref) FDetailCategoryStruct& DetailCategory, FDetailItemString StringItem);
	UFUNCTION(BlueprintCallable)
		static void AddNumberItemToCategory(UPARAM(ref) FDetailCategoryStruct& DetailCategory, FDetailItemNumber NumberItem);
	UFUNCTION(BlueprintCallable)
		static void AddCustomItemToCategory(UPARAM(ref) FDetailCategoryStruct& DetailCategory, FDetailItemCustom CustomItem);
	//Fill Filter by file extension,if the first element include character '/',it will be treat as MIME Type.
};
