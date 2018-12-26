// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMBPLib.h"
#include "Kismet/GameplayStatics.h"
#include "MuthMInGameMode.h"
#include "MuthMType.h"
#include "Engine/Texture2D.h"
#include "IImageWrapperModule.h"
#include "FileHelper.h"
#include "ModuleManager.h"
#include "IImageWrapper.h"
#include "Sound/SoundWave.h"
#include "MuthMNativeLib.h"
#include "Type/VisualizableSoundWave.h"
#include "TargetPlatform/Public/Interfaces/IAudioFormat.h"
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
	auto* InGameMode = Cast<AMuthMInGameMode>(UGameplayStatics::GetGameMode(WorldContextObj));
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
	if (FMemory::Memcmp(ImageData.GetData(), PNGHeader, sizeof(PNGHeader)))
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	else if (FMemory::Memcmp(ImageData.GetData(), SOI, sizeof(SOI))) //No Need to check EOI
		imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	else if (FMemory::Memcmp(ImageData.GetData(), "BM", 2))//Length of "BM" is 2
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
	TSharedPtr<FJsonObject> JsonObj = MakeShareable<FJsonObject>(new FJsonObject());
	TSharedRef<TJsonReader<TCHAR>> jsonr= TJsonReaderFactory<TCHAR>::Create(*JsonStr);
	FJsonSerializer::Deserialize(jsonr, JsonObj);
	return JsonObj;
}

class USoundWave* UMuthMBPLib::DecodeWaveFromOGG(const TArray<uint8>& OGGData)
{
	USoundWave* TargetSoundWave = NewObject<USoundWave>();
	TargetSoundWave->bVirtualizeWhenSilent = true;
	FByteBulkData* bulkData = &TargetSoundWave->CompressedFormatData.GetFormat(TEXT("OGG"));
	bulkData->Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(bulkData->Realloc(OGGData.Num()), OGGData.GetData(), OGGData.Num());
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
	return TargetSoundWave;
}

class UVisualizableSoundWave* UMuthMBPLib::DecodeVisualizableWaveFromOGG(const TArray<uint8>& OGGData)
{
	auto* TargetSoundWave = NewObject<UVisualizableSoundWave>();
	TargetSoundWave->bVirtualizeWhenSilent = true;
	FByteBulkData* bulkData = &TargetSoundWave->CompressedFormatData.GetFormat(TEXT("OGG"));
	bulkData->Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(bulkData->Realloc(OGGData.Num()), OGGData.GetData(), OGGData.Num());
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
	//Gen PCM Data
	TargetSoundWave->_CachedStdPCM.SetNum(soundQualityInfo.SampleDataSize);
	oggAudioInfo.ExpandFile(TargetSoundWave->_CachedStdPCM.GetData(), &soundQualityInfo);
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

bool UMuthMBPLib::GetOpenFileName(const FText& Title, TArray<FString> Filters, bool AllowMultipleSelected, TArray<FString>& SelectedFileName, const FString& InitDir/*=""*/)
{
	return FGameHAL::Get().OpenFileDialog(Title.ToString(), InitDir, Filters, AllowMultipleSelected, SelectedFileName);
}
