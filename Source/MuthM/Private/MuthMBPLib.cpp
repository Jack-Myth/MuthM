// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMBPLib.h"
#include "Kismet/GameplayStatics.h"
#include "InGameMode.h"
#include "MuthMType.h"
#include "Engine/Texture2D.h"
#include "IImageWrapperModule.h"
#include "FileHelper.h"
#include "ModuleManager.h"
#include "IImageWrapper.h"
#include "Sound/SoundWave.h"
#include "MuthMNativeLib.h"
#include "MainSoundWave.h"
#include "JsonReader.h"
#include "JsonSerializer.h"
#include "InstructionManager.h"
#include "ModManager.h"
#include "MusicManager.h"
#include "DownloadManager.h"
#include "NetworkManager.h"
#include "UserManager.h"
#include "GameHAL.h"
#include "VorbisAudioInfo.h"
#include "Engine/Engine.h"
#include "AudioDevice.h"

DEFINE_LOG_CATEGORY(MuthMBPLib)

#ifdef _MUTHM_USE_FMOD
#include "MainSoundWave/MainSWPlayerImplFMod.h"
#define MAINSWPLAYER_CLASS UMainSWPlayerImplFMod
#else
#include "MainSoundWave/MainSWPlayerImpl.h"
#define MAINSWPLAYER_CLASS UMainSWPlayerImpl
#endif // _MUTHM_USE_FMOD
#include "MainSWPlayer.h"
#include "Engine/LevelStreamingDynamic.h"

TScriptInterface<IInstructionManager> UMuthMBPLib::GetInstructionManager(const UObject* WorldContextObj)
{
	return IInstructionManager::Get(WorldContextObj);
}

TScriptInterface<IModManager> UMuthMBPLib::GetModManager(const UObject* WorldContextObj)
{
	return IModManager::Get(WorldContextObj);
}

TScriptInterface<class INetworkManager> UMuthMBPLib::GetNetworkManager(const UObject* WorldContextObj)
{
	return INetworkManager::Get(WorldContextObj);
}

TScriptInterface<class IDownloadManager> UMuthMBPLib::GetDownloadManager(const UObject* WorldContextObj)
{
	return IDownloadManager::Get(WorldContextObj);
}

TScriptInterface<class IMusicManager> UMuthMBPLib::GetMusicManager(const UObject* WorldContextObj)
{
	return IMusicManager::Get(WorldContextObj);
}

TScriptInterface<class IUserManager> UMuthMBPLib::GetUserManager(const UObject* WorldContextObj)
{
	return IUserManager::Get(WorldContextObj);
}

class UTexture2D* UMuthMBPLib::LoadTextureInGame(UObject* WorldContextObj, FString FileName)
{
	auto* InGameMode = Cast<AInGameMode>(UGameplayStatics::GetGameMode(WorldContextObj));
	if (IsValid(InGameMode))
	{
		TSharedPtr<FMDATFile> _MDAT = InGameMode->GetMDAT();
		auto FileData = _MDAT->GetFileData(FileName);
		return GetLocalTextureByImageData(FileData);
	}
	return nullptr; //No able to gen Texture2D
}

UTexture2D* UMuthMBPLib::GetLocalTexture(const FString &_TexPath)
{
	TArray<uint8> OutArray;
	if (FFileHelper::LoadFileToArray(OutArray, *_TexPath))
	{
		return GetLocalTextureByImageData(OutArray);
	}
	return nullptr;
}

UTexture2D* UMuthMBPLib::GetLocalTextureByImageData(const TArray<uint8>& ImageData)
{
	//89 50 4E 47 0D 0A 1A 0A 
	uint8 PNGHeader[8];
	PNGHeader[0] = 0x89;
	PNGHeader[1] = 0x50;
	PNGHeader[2] = 0x4E;
	PNGHeader[3] = 0x47;
	PNGHeader[4] = 0x0D;
	PNGHeader[5] = 0x0A;
	PNGHeader[6] = 0x1A;
	PNGHeader[7] = 0x0A;
	//JPG SOI
	uint8 SOI[2];
	SOI[0] = 0xFF;
	SOI[1] = 0xD8;
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> imageWrapper;
	UTexture2D* OutTex = NULL;
	//Check Image Format
	if (!FMemory::Memcmp(ImageData.GetData(), PNGHeader, sizeof(PNGHeader)))
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	else if (!FMemory::Memcmp(ImageData.GetData(), SOI, sizeof(SOI))) //No Need to check EOI
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	else if (!FMemory::Memcmp(ImageData.GetData(), "BM", 2))//Length of "BM" is 2
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	else
		return nullptr; //Unidentified Image Format
	if (imageWrapper.IsValid() &&
		imageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
	{
		const TArray<uint8>* uncompressedRGBA = NULL;
		if (imageWrapper->GetRaw(ERGBFormat::RGBA, 8, uncompressedRGBA))
		{
			const TArray<FColor> uncompressedFColor = uint8ToFColor(*uncompressedRGBA);
			OutTex = TextureFromImage(
				imageWrapper->GetWidth(),
				imageWrapper->GetHeight(),
				uncompressedFColor,
				true);
		}
	}
	return OutTex;
}

TArray<FColor> UMuthMBPLib::uint8ToFColor(const TArray<uint8> origin)
{
	TArray<FColor> uncompressedFColor;
	uint8 auxOrigin;
	FColor auxDst;

	for (int i = 0; i < origin.Num(); i++)
	{
		auxOrigin = origin[i];
		auxDst.R = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.G = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.B = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.A = auxOrigin;
		uncompressedFColor.Add(auxDst);
	}

	return uncompressedFColor;
}
UTexture2D* UMuthMBPLib::TextureFromImage(const int32 SrcWidth, const int32 SrcHeight, const TArray<FColor>&SrcData, const bool UseAlpha)
{
	UTexture2D* MyScreenshot = UTexture2D::CreateTransient(SrcWidth, SrcHeight, PF_R8G8B8A8);
	uint8* MipData = static_cast<uint8*>(MyScreenshot->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	uint8* DestPtr = NULL;
	const FColor* SrcPtr = NULL;
	for (int32 y = 0; y < SrcHeight; y++)
	{
		DestPtr = &MipData[(SrcHeight - 1 - y)*SrcWidth * sizeof(FColor)];
		SrcPtr = const_cast<FColor*>(&SrcData[(SrcHeight - 1 - y)*SrcWidth]);
		for (int32 x = 0; x < SrcWidth; x++)
		{
			*DestPtr++ = SrcPtr->R;
			*DestPtr++ = SrcPtr->G;
			*DestPtr++ = SrcPtr->B;
			if (UseAlpha)
			{
				*DestPtr++ = SrcPtr->A;
			}
			else
			{
				*DestPtr++ = 0xFF;
			}
			SrcPtr++;
		}
	}
	MyScreenshot->PlatformData->Mips[0].BulkData.Unlock();
	MyScreenshot->UpdateResource();

	return MyScreenshot;
}

FBlueprintJsonObject UMuthMBPLib::LoadJsonFromStr(FString JsonStr)
{
	FBlueprintJsonObject tmpJsonObj;
	tmpJsonObj.Object = DeserializeJsonFromStr(JsonStr);
	return tmpJsonObj;
}

TSharedPtr<FJsonObject> UMuthMBPLib::DeserializeJsonFromUTF8(const TArray<uint8>& CharArray)
{
	FString JsonStr(UTF8_TO_TCHAR((const ANSICHAR*)CharArray.GetData()));
	return DeserializeJsonFromStr(JsonStr);
}

TSharedPtr<FJsonObject> UMuthMBPLib::DeserializeJsonFromStr(FString JsonStr)
{
	//JsonStr may have dirty tail(UTF8_TO_CHAR error),so clear tails until '}'
	int i = JsonStr.Len();
	while (i > 0 && JsonStr[i-- - 1] != TEXT('}'));
	JsonStr=JsonStr.Mid(0, i + 1);
	TSharedPtr<FJsonObject> JsonObj = MakeShareable<FJsonObject>(new FJsonObject());
	TSharedRef<TJsonReader<TCHAR>> jsonr= TJsonReaderFactory<TCHAR>::Create(*JsonStr);
	FJsonSerializer::Deserialize(jsonr, JsonObj);
	return JsonObj;
}

class USoundWave* UMuthMBPLib::DecodeWaveFromOGG(const TArray<uint8>& OGGData)
{
	USoundWave* TargetSoundWave = NewObject<USoundWave>();
	FByteBulkData* bulkData = &TargetSoundWave->CompressedFormatData.GetFormat(TEXT("OGG"));
	UE_LOG(MuthMBPLib, Log, TEXT("OGGData Length:%d"), OGGData.Num());
	bulkData->Lock(LOCK_READ_WRITE);
	void* pbulkData = bulkData->Realloc(OGGData.Num());
	FMemory::Memcpy(pbulkData, OGGData.GetData(), OGGData.Num());
	if (FMemory::Memcmp(pbulkData, OGGData.GetData(), OGGData.Num()))
		UE_LOG(MuthMBPLib, Error, TEXT("Bulk Data is not equal to OGGData!"));
	bulkData->Unlock();
	FSoundQualityInfo soundQualityInfo;
	FVorbisAudioInfo oggAudioInfo;
	if (!oggAudioInfo.ReadCompressedInfo(OGGData.GetData(), OGGData.Num(), &soundQualityInfo))
	{
		UE_LOG(MuthMBPLib, Error, TEXT("Unable to read OGG Info"));
		return nullptr;
	}
	TargetSoundWave->SoundGroup = ESoundGroup::SOUNDGROUP_Default;
	TargetSoundWave->NumChannels = soundQualityInfo.NumChannels;
	TargetSoundWave->Duration = soundQualityInfo.Duration;
	TargetSoundWave->RawPCMDataSize = soundQualityInfo.SampleDataSize;
	TargetSoundWave->SetSampleRate(soundQualityInfo.SampleRate);
	TargetSoundWave->bVirtualizeWhenSilent = true;
#if !defined(PLATFORM_WINDOWS)||!PLATFORM_WINDOWS
	TargetSoundWave->DecompressionType = EDecompressionType::DTYPE_Setup;
	TargetSoundWave->RawPCMData = (uint8*)FMemory::Malloc(TargetSoundWave->RawPCMDataSize);
	oggAudioInfo.ExpandFile(TargetSoundWave->RawPCMData, &soundQualityInfo);
#endif
	return TargetSoundWave;
}

bool UMuthMBPLib::ConvertMP3ToOGG(const TArray<uint8>& MP3File, TArray<uint8>& OGGOutput)
{
	TArray<uint8> AudioPCM;
	int32 SampleRate;
	int32 Channels;
	if (!MuthMNativeLib::NativeDecodeMP3ToPCM(MP3File, AudioPCM,SampleRate, Channels))
		return false;
	return MuthMNativeLib::NativeEncodePCMToOGG(AudioPCM,SampleRate,Channels,OGGOutput);
}

void UMuthMBPLib::AddStringItemToCategory(FDetailCategoryStruct& DetailCategory, FDetailItemString StringItem)
{
	FDetailItemString* mDetail = new FDetailItemString();
	*mDetail = StringItem;
	DetailCategory.ItemList.Add(MakeShareable<FDetailItem>(mDetail));
}

void UMuthMBPLib::AddNumberItemToCategory(FDetailCategoryStruct& DetailCategory, FDetailItemNumber NumberItem)
{
	FDetailItemNumber* mDetail = new FDetailItemNumber();
	*mDetail = NumberItem;
	DetailCategory.ItemList.Add(MakeShareable<FDetailItem>(mDetail));
}

void UMuthMBPLib::AddCustomItemToCategory(FDetailCategoryStruct& DetailCategory, FDetailItemCustom CustomItem)
{
	FDetailItemCustom* mDetail = new FDetailItemCustom();
	*mDetail = CustomItem;
	DetailCategory.ItemList.Add(MakeShareable<FDetailItem>(mDetail));
}

TScriptInterface<IMainSWPlayer> UMuthMBPLib::GenMainSWPlayer(const UObject* WorldContextObj)
{
	return NewObject<MAINSWPLAYER_CLASS>(WorldContextObj->GetWorld());
}

ULevelStreamingDynamic* UMuthMBPLib::GenStreamingLevelInstance(class UObject* WorldContextObj, const FString& MapName, const FVector& Location, const FRotator& Rotation)
{
	FString LongPackageName;
	if (!FPackageName::SearchForPackageOnDisk(MapName, &LongPackageName))
		return nullptr;
	// Create Unique Name for sub-level package
	const FString ShortPackageName = FPackageName::GetShortName(LongPackageName);
	const FString PackagePath = FPackageName::GetLongPackagePath(LongPackageName);
	FString LevelPackageName = PackagePath + TEXT("/") + WorldContextObj->GetWorld()->StreamingLevelsPrefix + ShortPackageName;
	//LevelPackageName += TEXT("_LevelInstance_");
	ULevelStreamingDynamic* LevelInstance = NewObject<ULevelStreamingDynamic>(WorldContextObj->GetWorld(), NAME_None);
	LevelInstance->SetWorldAssetByPackageName(*LevelPackageName);
	LevelInstance->SetShouldBeLoaded(true);
	LevelInstance->SetShouldBeVisible(true);
	LevelInstance->bInitiallyLoaded = true;
	LevelInstance->bInitiallyVisible = true;
	LevelInstance->bShouldBlockOnLoad = true;
	LevelInstance->PackageNameToLoad = *LongPackageName;
	LevelInstance->LevelColor = FColor::MakeRandomColor();
	LevelInstance->LevelTransform = FTransform(Rotation, Location);
	return LevelInstance;
}