// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "ChartImporterCytus.h"
#include "FileHelper.h"
#include "InstructionManager.h"
#include "BlueprintJsonLibrary.h"
#include "MuthMBPLib.h"
#include "Instruction.h"

DEFINE_LOG_CATEGORY(ChartImporterCytus)

bool UChartImporterCytus::ImportFromFile(FString FilePath, TArray<class UInstruction *>& ImportResult)
{
	TArray<FString> FileContents;
	if (!FFileHelper::LoadFileToStringArray(FileContents, *FilePath))
	{
		UE_LOG(ChartImporterCytus,Error, TEXT("Can't load chart file from %s"), *FilePath);
		return false;
	}
	//Check Cytus Chart Version
	if (FileContents[0]!="VERSION 2")
	{
		UE_LOG(ChartImporterCytus, Error, TEXT("Unsupport Cytus chart file, Only Cytus Chart V2 is supported!"));
		return false;
	}
	struct CytusNoteInfo
	{
		float Time;
		float Position = 0;
		float PressLength = 0;
		bool bIsLinked = true;
	};
	TArray<CytusNoteInfo> Notes;
	TArray<TArray<int32>> LinkedNotes;
	int32 CurrentLine = 4; //Ignore BPM,PAGE_SHIFT and PAGE_SIZE
	for (; CurrentLine < FileContents.Num(); CurrentLine++)
	{
		//Process NOTE
		TArray<FString> RecordParts;
		static const TCHAR* Spliter[] = { TEXT("\t"),TEXT(" ") };
		FileContents[CurrentLine].ParseIntoArray(RecordParts, Spliter, 2);
		if (RecordParts.Num()==0||RecordParts[0]!="NOTE")
			break;
		int32 NoteIndex = FCString::Atoi(*RecordParts[1]);
		CytusNoteInfo tmpNoteInfo;
		tmpNoteInfo.Time = FCString::Atof(*RecordParts[2]);
		tmpNoteInfo.Position = FCString::Atof(*RecordParts[3]);
		tmpNoteInfo.PressLength = FCString::Atof(*RecordParts[4]);
		tmpNoteInfo.bIsLinked = false;
		if (NoteIndex >= Notes.Num())
			Notes.SetNum(NoteIndex+1);
		Notes[NoteIndex] = tmpNoteInfo;
	}
	for (;CurrentLine<FileContents.Num();CurrentLine++)
	{
		//Process LINK
		TArray<FString> RecordParts;
		static const TCHAR* Spliter[] = { TEXT("\t"),TEXT(" ") };
		FileContents[CurrentLine].ParseIntoArray(RecordParts, Spliter, 2);
		if (RecordParts.Num() == 0 || FileContents[CurrentLine] != "LINK")
			break;
		TArray<int32> curLinkedNote;
		for (int i=1;i<RecordParts.Num();i++)
		{
			int32 NoteIndex = FCString::Atoi(*RecordParts[i]);
			CytusNoteInfo* curNote = &Notes[NoteIndex];
			if (curNote->bIsLinked!=false)
			{
				UE_LOG(ChartImporterCytus, Warning, TEXT("Invalid Note Index(%d), Abort Importation!"), NoteIndex);
				return false;
			}
			curNote->bIsLinked = true;
			curLinkedNote.Add(NoteIndex);
		}
		if (curLinkedNote.Num())
			LinkedNotes.Add(curLinkedNote);
	}

	//Generate Instructions;
	auto InstructionManager = IInstructionManager::Get(this);
	for (int i=0;i<Notes.Num();i++)
	{
		if (Notes[i].bIsLinked)
			continue;
		if (Notes[i].PressLength == 0)
		{
			//Normal Note
			UInstruction* CurNote = InstructionManager->GenInstruction("RhythmTap", Notes[i].Time);
			FBlueprintJsonObject Args;
			Args.Object = MakeShareable(new FJsonObject());
			Args.Object->SetNumberField("LROffset", (Notes[i].Position - 0.5f) * 2);
			UMuthMBPLib::SaveColorToJson(Args.Object, "Color", FLinearColor(0.4f, 0.8f, 1.f));
			CurNote->OnInstructionLoaded_Editor(Args, FEditorExtraInfo());
			ImportResult.Add(CurNote);
		}
		else
		{
			//Need Press
			UInstruction* CurNote = InstructionManager->GenInstruction("RhythmSlide", Notes[i].Time);
			FBlueprintJsonObject Args;
			Args.Object = MakeShareable(new FJsonObject());
			Args.Object->SetNumberField("LROffset", (Notes[i].Position - 0.5f) * 2);
			UMuthMBPLib::SaveColorToJson(Args.Object, "Color", FLinearColor(0.4f, 0.8f, 1.f));
			Args.Object->SetNumberField("SubNotesCount", 1);
			TSharedPtr<FJsonObject> SubNoteInfo = MakeShareable(new FJsonObject());
			SubNoteInfo->SetNumberField("TimeOffset", Notes[i].PressLength);
			SubNoteInfo->SetNumberField("PositionOffset", 0);
			SubNoteInfo->SetNumberField("Score", 150.0);
			Args.Object->SetObjectField("SubNote0", SubNoteInfo);
			CurNote->OnInstructionLoaded_Editor(Args, FEditorExtraInfo());
			ImportResult.Add(CurNote);
		}
	}
	for (int i=0;i<LinkedNotes.Num();i++)
	{
		LinkedNotes[i].Sort([&](int32 a, int32 b) {return Notes[LinkedNotes[i][a]].Time < Notes[LinkedNotes[i][b]].Time; });
		UInstruction* CurNote = InstructionManager->GenInstruction("RhythmSlide", Notes[LinkedNotes[i][0]].Time);
		FBlueprintJsonObject Args;
		Args.Object = MakeShareable(new FJsonObject());
		Args.Object->SetNumberField("LROffset", (Notes[LinkedNotes[i][0]].Position - 0.5f) * 2);
		UMuthMBPLib::SaveColorToJson(Args.Object, "Color", FLinearColor(0.4f, 0.8f, 1.f));
		Args.Object->SetNumberField("SubNotesCount", LinkedNotes.Num() - 1);
		for (int k = 1; k < LinkedNotes[i].Num(); k++)
		{
			TSharedPtr<FJsonObject> SubNoteInfo = MakeShareable(new FJsonObject());
			SubNoteInfo->SetNumberField("TimeOffset", Notes[LinkedNotes[i][k]].Time - Notes[LinkedNotes[i][0]].Time);
			float PositionOffset = Notes[LinkedNotes[i][k]].Position - Notes[LinkedNotes[i][0]].Position;
			SubNoteInfo->SetNumberField("PositionOffset", PositionOffset);
			SubNoteInfo->SetNumberField("Score", 50.0);
			Args.Object->SetObjectField(FString::Printf(TEXT("SubNote%d"), k - 1), SubNoteInfo);
		}
		CurNote->OnInstructionLoaded_Editor(Args, FEditorExtraInfo());
		ImportResult.Add(CurNote);
	}
	Algo::Sort(ImportResult, [](UInstruction* a, UInstruction* b) {return a->GetTime() < b->GetTime(); });
	return true;
}
