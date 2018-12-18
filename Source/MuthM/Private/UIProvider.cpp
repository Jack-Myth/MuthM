// Copyright (C) 2018 JackMyth. All Rights Reserved. 
// Code is generated by PreBuildSteps, Don't modify it directly, Use UIRec.txt instead. 
 
#include "UIProvider.h" 
#include "PauseUIBase.h" 
#include "DetailsListBase.h" 
#include "DetailCategoryBase.h" 
#include "DetailInputNumberBase.h" 
#include "DetailInputStringBase.h" 
#include "EditorMainUIBase.h" 
#include "EditorPanelBase.h" 
#include "InstructionWidgetBase.h" 
#include "GameUIBase.h" 
#include "GameResultUIBase.h" 
#include "WelcomeUIBase.h" 
#include "MainMenuUIBase.h" 
#include "ScoreSelectionUIBase.h" 
#include "GenericSelectionItemBase.h" 
#include "MessageBoxBase.h" 
#include "QuestionBoxBase.h" 
#include "DownloadItemBase.h" 
#include "UploadItemBase.h" 
#include "DownloadListBase.h" 
#include "ContentManagerUIBase.h" 
#include "MusicManagerUIBase.h" 
#include "ScoreManagerUIBase.h" 
 
TSubclassOf<UPauseUIBase> UUIProvider::GetPauseUI() 
{ 
	if(PauseUI.IsBound()) 
	{ 
		TSubclassOf<UPauseUIBase> _uclass; 
		PauseUI.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UPauseUIBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/PauseUI.PauseUI_C'"));
	} 
} 
 
TSubclassOf<UDetailsListBase> UUIProvider::GetDetailsList() 
{ 
	if(DetailsList.IsBound()) 
	{ 
		TSubclassOf<UDetailsListBase> _uclass; 
		DetailsList.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UDetailsListBase>(NULL,TEXT("WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/DetailsList.DetailsList_C'"));
	} 
} 
 
TSubclassOf<UDetailCategoryBase> UUIProvider::GetDetailCategory() 
{ 
	if(DetailCategory.IsBound()) 
	{ 
		TSubclassOf<UDetailCategoryBase> _uclass; 
		DetailCategory.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UDetailCategoryBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/DetailCategory.DetailCategory_C'"));
	} 
} 
 
TSubclassOf<UDetailInputNumberBase> UUIProvider::GetDetailInputNumber() 
{ 
	if(DetailInputNumber.IsBound()) 
	{ 
		TSubclassOf<UDetailInputNumberBase> _uclass; 
		DetailInputNumber.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UDetailInputNumberBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/DetailInputNumber.DetailInputNumber_C'"));
	} 
} 
 
TSubclassOf<UDetailInputStringBase> UUIProvider::GetDetailInputString() 
{ 
	if(DetailInputString.IsBound()) 
	{ 
		TSubclassOf<UDetailInputStringBase> _uclass; 
		DetailInputString.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UDetailInputStringBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/DetailInputString.DetailInputString_C'"));
	} 
} 
 
TSubclassOf<UEditorMainUIBase> UUIProvider::GetEditorMainUI() 
{ 
	if(EditorMainUI.IsBound()) 
	{ 
		TSubclassOf<UEditorMainUIBase> _uclass; 
		EditorMainUI.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UEditorMainUIBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/EditorMainUI.EditorMainUI_C'"));
	} 
} 
 
TSubclassOf<UEditorPanelBase> UUIProvider::GetEditorPanel() 
{ 
	if(EditorPanel.IsBound()) 
	{ 
		TSubclassOf<UEditorPanelBase> _uclass; 
		EditorPanel.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UEditorPanelBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/EditorPanel.EditorPanel_C'"));
	} 
} 
 
TSubclassOf<UInstructionWidgetBase> UUIProvider::GetInstructionWidget() 
{ 
	if(InstructionWidget.IsBound()) 
	{ 
		TSubclassOf<UInstructionWidgetBase> _uclass; 
		InstructionWidget.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UInstructionWidgetBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/InstructionWidget.InstructionWidget_C'"));
	} 
} 
 
TSubclassOf<UGameUIBase> UUIProvider::GetGameUI() 
{ 
	if(GameUI.IsBound()) 
	{ 
		TSubclassOf<UGameUIBase> _uclass; 
		GameUI.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UGameUIBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/GameUI.GameUI_C'"));
	} 
} 
 
TSubclassOf<UGameResultUIBase> UUIProvider::GetGameResultUI() 
{ 
	if(GameResultUI.IsBound()) 
	{ 
		TSubclassOf<UGameResultUIBase> _uclass; 
		GameResultUI.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UGameResultUIBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/GameResultUI.GameResultUI_C'"));
	} 
} 
 
TSubclassOf<UWelcomeUIBase> UUIProvider::GetWelcomeUI() 
{ 
	if(WelcomeUI.IsBound()) 
	{ 
		TSubclassOf<UWelcomeUIBase> _uclass; 
		WelcomeUI.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UWelcomeUIBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/WelcomeUI.WelcomeUI_C'"));
	} 
} 
 
TSubclassOf<UMainMenuUIBase> UUIProvider::GetMainMenuUI() 
{ 
	if(MainMenuUI.IsBound()) 
	{ 
		TSubclassOf<UMainMenuUIBase> _uclass; 
		MainMenuUI.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UMainMenuUIBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/MainMenuUI.MainMenuUI_C'"));
	} 
} 
 
TSubclassOf<UScoreSelectionUIBase> UUIProvider::GetScoreSelectionUI() 
{ 
	if(ScoreSelectionUI.IsBound()) 
	{ 
		TSubclassOf<UScoreSelectionUIBase> _uclass; 
		ScoreSelectionUI.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UScoreSelectionUIBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/ScoreSelectionUI.ScoreSelectionUI_C'"));
	} 
} 
 
TSubclassOf<UGenericSelectionItemBase> UUIProvider::GetGenericSelectionItem() 
{ 
	if(GenericSelectionItem.IsBound()) 
	{ 
		TSubclassOf<UGenericSelectionItemBase> _uclass; 
		GenericSelectionItem.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UGenericSelectionItemBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/GenericSelectionItem.GenericSelectionItem_C'"));
	} 
} 
 
TSubclassOf<UMessageBoxBase> UUIProvider::GetMessageBox() 
{ 
	if(MessageBox.IsBound()) 
	{ 
		TSubclassOf<UMessageBoxBase> _uclass; 
		MessageBox.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UMessageBoxBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/MessageBox.MessageBox_C'"));
	} 
} 
 
TSubclassOf<UQuestionBoxBase> UUIProvider::GetQuestionBox() 
{ 
	if(QuestionBox.IsBound()) 
	{ 
		TSubclassOf<UQuestionBoxBase> _uclass; 
		QuestionBox.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UQuestionBoxBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/QuestionBox.QuestionBox_C'"));
	} 
} 
 
TSubclassOf<UDownloadItemBase> UUIProvider::GetDownloadItem() 
{ 
	if(DownloadItem.IsBound()) 
	{ 
		TSubclassOf<UDownloadItemBase> _uclass; 
		DownloadItem.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UDownloadItemBase>(NULL,TEXT("WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/DownloadItem.DownloadItem_C'"));
	} 
} 
 
TSubclassOf<UUploadItemBase> UUIProvider::GetUploadItem() 
{ 
	if(UploadItem.IsBound()) 
	{ 
		TSubclassOf<UUploadItemBase> _uclass; 
		UploadItem.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UUploadItemBase>(NULL,TEXT("WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/UploadItem.UploadItem_C'"));
	} 
} 
 
TSubclassOf<UDownloadListBase> UUIProvider::GetDownloadList() 
{ 
	if(DownloadList.IsBound()) 
	{ 
		TSubclassOf<UDownloadListBase> _uclass; 
		DownloadList.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UDownloadListBase>(NULL,TEXT("WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/DownloadList.DownloadList_C'"));
	} 
} 
 
TSubclassOf<UContentManagerUIBase> UUIProvider::GetContentManagerUI() 
{ 
	if(ContentManagerUI.IsBound()) 
	{ 
		TSubclassOf<UContentManagerUIBase> _uclass; 
		ContentManagerUI.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UContentManagerUIBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/ContentManagerUI.ContentManagerUI_C'"));
	} 
} 
 
TSubclassOf<UMusicManagerUIBase> UUIProvider::GetMusicManagerUI() 
{ 
	if(MusicManagerUI.IsBound()) 
	{ 
		TSubclassOf<UMusicManagerUIBase> _uclass; 
		MusicManagerUI.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UMusicManagerUIBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/MusicManagerUI.MusicManagerUI_C'"));
	} 
} 
 
TSubclassOf<UScoreManagerUIBase> UUIProvider::GetScoreManagerUI() 
{ 
	if(ScoreManagerUI.IsBound()) 
	{ 
		TSubclassOf<UScoreManagerUIBase> _uclass; 
		ScoreManagerUI.Execute(_uclass);
		return _uclass;
	} 
	else
	{
		return LoadClass<UScoreManagerUIBase>(NULL,TEXT( "WidgetBlueprint'/Game/MuthM/Blueprints/UI/GameFramework/ScoreManagerUI.ScoreManagerUI_C'"));
	} 
} 
 
