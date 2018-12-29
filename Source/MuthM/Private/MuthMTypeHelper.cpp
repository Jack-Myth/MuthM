// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "MuthMTypeHelper.h"
#include "MuthMType.h"
#include "MuthMBPLib.h"
#include "JsonObject.h"
#include "JsonValue.h"
#include "JsonReader.h"
#include "JsonWriter.h"
#include "JsonSerializer.h"

bool MuthMTypeHelper::TryGetMDATMainInfo(class FMDATFile* MDATFile, FMDATMainInfo& MainInfo)
{
	if (!MDATFile)
		return false;
	TArray<uint8> MainJson = MDATFile->GetFileData("/Main");
	if (!MainJson.Num())
		return false;
	TSharedPtr<FJsonObject> MainJsonObj = UMuthMBPLib::DeserializeJsonFromUTF8(MainJson);
	MainInfo = FMDATMainInfo();
	MainInfo.Title = MainJsonObj->GetStringField("Title");
	MainInfo.AuthorID = MainJsonObj->GetIntegerField("AuthorId");
	MainInfo.AuthorName = MainJsonObj->GetStringField("AuthorName");
	MainInfo.Description = MainJsonObj->GetStringField("Description");
	MainInfo.Cover = MainJsonObj->GetStringField("Cover");
	int ScoreCount = MainJsonObj->GetIntegerField("ScoreCount");
	for (int i=0;i<ScoreCount;i++)
	{
		FMusicScoreInfo tmpScoreInfo;
		TSharedPtr<FJsonObject> CurScore = MainJsonObj->GetObjectField(FString::Printf(TEXT("Score%d"), i));
		tmpScoreInfo.Title = CurScore->GetStringField("ScoreName");
		tmpScoreInfo.Subtitle = CurScore->GetStringField("Subtitle");
		tmpScoreInfo.MusicID = CurScore->GetIntegerField("MusicID");
		tmpScoreInfo.ScoreDataFileName = CurScore->GetStringField("ScoreData");
		tmpScoreInfo.Img = CurScore->GetStringField("Img");
		auto RequestMods = CurScore->GetArrayField("RequestMods");
		for (int ii=0;ii<RequestMods.Num();ii++)
			tmpScoreInfo.RequestMods.Add(*RequestMods[ii]->AsString());
		auto OptionalMods = CurScore->GetArrayField("OptionalMods");
		for (int ii = 0; ii < OptionalMods.Num(); ii++)
			tmpScoreInfo.OptionalMods.Add(*OptionalMods[ii]->AsString());
		MainInfo.ScoreInfoCollection.Add(tmpScoreInfo);
	}
	return true;
}

void MuthMTypeHelper::SaveMDATMainInfo(class FMDATFile* MDATFile, const FMDATMainInfo& MainInfo)
{
	if (!MDATFile)
		return;
	FString MainJsonStr;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&MainJsonStr);
	FJsonObject tmpMainJsonObj;
	tmpMainJsonObj.SetStringField("Title", MainInfo.Title);
	tmpMainJsonObj.SetStringField("Description", MainInfo.Description);
	tmpMainJsonObj.SetNumberField("AuthorID", MainInfo.AuthorID);
	tmpMainJsonObj.SetStringField("AuthorName", MainInfo.AuthorName);
	tmpMainJsonObj.SetStringField("Cover", MainInfo.Cover);
	tmpMainJsonObj.SetNumberField("ScoreCount", MainInfo.ScoreInfoCollection.Num());
	for (int i=0;i<MainInfo.ScoreInfoCollection.Num();i++)
	{
		TSharedPtr<FJsonObject> tmpScoreJsonObj = MakeShareable(new FJsonObject());
		tmpScoreJsonObj->SetStringField("ScoreName", MainInfo.ScoreInfoCollection[i].Title);
		tmpScoreJsonObj->SetStringField("Subtitle", MainInfo.ScoreInfoCollection[i].Subtitle);
		tmpScoreJsonObj->SetStringField("ScoreData", MainInfo.ScoreInfoCollection[i].ScoreDataFileName);
		tmpScoreJsonObj->SetStringField("Img", MainInfo.ScoreInfoCollection[i].Img);
		tmpScoreJsonObj->SetNumberField("MusicID", MainInfo.ScoreInfoCollection[i].MusicID);
		TArray<TSharedPtr<FJsonValue>> RequestMods;
		for (int ii = 0; ii < MainInfo.ScoreInfoCollection[i].RequestMods.Num(); ii++)
			RequestMods.Add(MakeShareable(new FJsonValueString(MainInfo.ScoreInfoCollection[i].RequestMods[ii].ToString())));
		tmpScoreJsonObj->SetArrayField("RequestMods", RequestMods);
		TArray<TSharedPtr<FJsonValue>> OptionalMods;
		for (int ii = 0; ii < MainInfo.ScoreInfoCollection[i].OptionalMods.Num(); ii++)
			OptionalMods.Add(MakeShareable(new FJsonValueString(MainInfo.ScoreInfoCollection[i].OptionalMods[ii].ToString())));
		tmpScoreJsonObj->SetArrayField("OptionalMods", OptionalMods);
		tmpMainJsonObj.SetObjectField(FString::Printf(TEXT("Score%d"), i),tmpScoreJsonObj);
	}
	TSharedRef<FJsonObject> JsonObjRef = MakeShared<FJsonObject>(tmpMainJsonObj);
	FJsonSerializer::Serialize(JsonObjRef, JsonWriter);
	TArray<uint8> JsonCharArray;
	FTCHARToUTF8 T2UConvertion(*MainJsonStr);
	JsonCharArray.Append((const uint8*)T2UConvertion.Get(), T2UConvertion.Length());
	MDATFile->RemoveFile("/Main");
	MDATFile->AddFile("/Main", JsonCharArray);
	MDATFile->Save();
}
