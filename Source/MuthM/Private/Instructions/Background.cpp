// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "Background.h"
#include "UIProvider.h"
#include "DetailInputSelectionBase.h"
#include "BlueprintJsonLibrary.h"
#include "MuthMBPLib.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "MMScript.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObject.h"

#define LOCTEXT_NAMESPACE "MuthM"

void UBackground::OnStrValueCallback(class UInstruction* InstructionInstance, FName PropertyName, FString StrValue)
{
	if (PropertyName=="ImageFileName")
	{
		ImageFileName = StrValue;
		//No Widget need to be updated.
	}
}

void UBackground::OnSelectionValueCallback(class UInstruction* InstructionInstance, FName PropertyName, class UDetailInputCustomBase* CustomWidget)
{
	auto* SelectionWidget = Cast<UDetailInputSelectionBase>(CustomWidget);
	if (PropertyName=="Operation")
	{
		Operation = SelectionWidget->GetCurrentSelection();
	}
	else if (PropertyName == "Animation")
	{
		TargetAnimation = SelectionWidget->GetCurrentSelection();
	}
}

void UBackground::OnNumberValueCallback(class UInstruction* InstructionInstance, FName PropertyName, float NumberValue)
{
	if (PropertyName=="AnimationLength")
	{
		AnimationLength = NumberValue;
	}
}

void UBackground::InitProperties(FBlueprintJsonObject Args)
{
	TargetAnimation=Args.Object->GetStringField("TargetAnimation");
	if (TargetAnimation == "")
		TargetAnimation = "None";	
	Operation = Args.Object->GetStringField("Operation");
	if (Operation == "")
		Operation = "Show";
	ImageFileName=Args.Object->GetStringField("ImageFileName");
	AnimationLength = Args.Object->GetNumberField("AnimationLength");
}

UBackground::UBackground()
{
	SupportedAnimation.Add("None");
	SupportedAnimation.Add("Fade");
}

void UBackground::OnBuildingDetails_Implementation(TScriptInterface<IDetailsBuilder>& DetailsBuilder)
{
	Super::OnBuildingDetails_Implementation(DetailsBuilder);
	
	FDetailCategoryStruct Background;
	Background.Title = "Background";
	Background.DisplayTitle = LOCTEXT("Background", "Background");

	//Operation
	TSharedPtr<FDetailItemCustom> OperationDetail = MakeShareable(new FDetailItemCustom());
	OperationDetail->Name = "Operation";
	OperationDetail->DisplayName = LOCTEXT("Operation", "Operation");
	OperationDetail->InstructionInstance = this;
	auto* OperationWidget = Cast<UDetailInputSelectionBase>(UUserWidget::CreateWidgetInstance(*GetWorld(),UUIProvider::Get(this)->GetDetailInputSelection(),NAME_None));
	TArray<FString> OpArray;
	OpArray.Add("Show");
	OpArray.Add("Hide");
	OperationWidget->FillOptions(OpArray);
	OperationWidget->SetCurrentSelection(Operation);
	OperationDetail->CustomWidget = OperationWidget;
	OperationDetail->DetailCallbackCustom.BindUFunction(this, "OnSelectionValueCallback");
	Background.ItemList.Add(OperationDetail);

	//Image File Name
	TSharedPtr<FDetailItemString> ImageFileNameDetail = MakeShareable(new FDetailItemString());
	ImageFileNameDetail->Name = "ImageFileName";
	ImageFileNameDetail->DisplayName = LOCTEXT("ImageFileName", "ImageFileName");
	ImageFileNameDetail->InstructionInstance = this;
	ImageFileNameDetail->StrValue = ImageFileName;
	ImageFileNameDetail->DetailCallbackStr.BindUFunction(this, "OnStrValueCallback");
	Background.ItemList.Add(ImageFileNameDetail);

	//Animation
	TSharedPtr<FDetailItemCustom> AnimationDetail = MakeShareable(new FDetailItemCustom());
	AnimationDetail->Name = "Animation";
	AnimationDetail->DisplayName = LOCTEXT("Animation", "Animation");
	AnimationDetail->InstructionInstance = this;
	auto* AnimationWidget = Cast<UDetailInputSelectionBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get(this)->GetDetailInputSelection(), NAME_None));
	AnimationWidget->FillOptions(SupportedAnimation);
	AnimationWidget->SetCurrentSelection(TargetAnimation);
	AnimationDetail->CustomWidget = AnimationWidget;
	AnimationDetail->DetailCallbackCustom.BindUFunction(this, "OnSelectionValueCallback");
	Background.ItemList.Add(AnimationDetail);

	//Animation Length
	TSharedPtr<FDetailItemNumber> AnimationLengthDetail = MakeShareable(new FDetailItemNumber());
	AnimationLengthDetail->Name = "AnimationLength";
	AnimationLengthDetail->DisplayName = LOCTEXT("AnimationLength", "AnimationLength");
	AnimationLengthDetail->InstructionInstance = this;
	AnimationLengthDetail->NumberValue = AnimationLength;
	AnimationLengthDetail->DetailCallbackNumber.BindUFunction(this, "OnNumberValueCallback");
	Background.ItemList.Add(AnimationLengthDetail);

	DetailsBuilder->AddCategory(Background);
}

void UBackground::OnTimeArrived_Implementation()
{
	//Get BGActor
	AStaticMeshActor* BGActor = Cast<AStaticMeshActor>(GetGlobalObjectData("_MuthM_Background_Actor"));
	//Set Position and rotation.
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this,0);
	AActor* ViewTargetActor = CameraManager->GetViewTarget();
	FMinimalViewInfo MVI;
	ViewTargetActor->CalcCamera(0.f, MVI);
	FVector TargetLocation = MVI.Location + MVI.Rotation.Quaternion().GetAxisX() * 3000;
	BGActor->SetActorLocation(TargetLocation);
	FRotator tmpRotation = FRotationMatrix::MakeFromZ(MVI.Location - TargetLocation).Rotator();
	BGActor->SetActorRotation(tmpRotation);
	BGActor->AddActorLocalRotation(FRotator(0, 180.f, 0));
	if (Operation=="Show")	
		BGActor->GetStaticMeshComponent()->SetVisibility(true);
	else if (Operation=="Hide")
		BGActor->GetStaticMeshComponent()->SetVisibility(false);
	//Rescale
	float Ratio = BackgroundImage->GetSizeX() / (float)BackgroundImage->GetSizeY();
	if (Ratio > 16.f / 9.f)
	{
		//Scale base on Y;
		BGActor->SetActorScale3D(FVector(Ratio*33.75f, 33.75f, 1.f));
	}
	else
	{
		//Scale base on X;
		BGActor->SetActorScale3D(FVector(60.f, 60.f / Ratio, 1.f));
	}

	//Apply Animation.
	if (TargetAnimation=="None")
	{
		auto* InstanceDynamic = Cast<UMaterialInstanceDynamic>(BGActor->GetStaticMeshComponent()->GetMaterial(0));
		if (!InstanceDynamic)
		{
			//If there have no DMI existed.
			UMaterial* MaterialTemplate = LoadObject<UMaterial>(this, TEXT("Material'/Game/MuthM/Materials/Game/Background/Fade.Fade'"));
			InstanceDynamic = UMaterialInstanceDynamic::Create(MaterialTemplate, nullptr, NAME_None);
			BGActor->GetStaticMeshComponent()->SetMaterial(0, InstanceDynamic);
		}
		//Force Ignore Animation
		InstanceDynamic->SetTextureParameterValue("BG1", BackgroundImage);
		InstanceDynamic->SetScalarParameterValue("AnimationAlpha", 1.f);
		CachedDMI = InstanceDynamic;
		AnimationProgress = 1.f;
	}
	else if (TargetAnimation == "Fade")
	{
		UMaterial* MaterialTemplate = LoadObject<UMaterial>(this, TEXT("Material'/Game/MuthM/Materials/Game/Background/Fade.Fade'"));
		auto* InstanceDynamic = UMaterialInstanceDynamic::Create(MaterialTemplate, nullptr, NAME_None);
		//Swap BG1 to next material's BG0
		if (Cast<UMaterialInstanceDynamic>(BGActor->GetStaticMeshComponent()->GetMaterial(0)))
		{
			UTexture* BG0;
			BGActor->GetStaticMeshComponent()->GetMaterial(0)->GetTextureParameterValue(FMaterialParameterInfo("BG1"), BG0);
			if (BG0->IsValidLowLevel())
				InstanceDynamic->SetTextureParameterValue("BG0", BG0);
		}
		BGActor->GetStaticMeshComponent()->SetMaterial(0, InstanceDynamic);
		InstanceDynamic->SetTextureParameterValue("BG1", BackgroundImage);
		InstanceDynamic->SetScalarParameterValue("AnimationAlpha", 0.f);
		CachedDMI = InstanceDynamic;
		AnimationProgress = 0.f;
	}
}

void UBackground::OnInstructionLoaded_Implementation(FBlueprintJsonObject Args)
{
	Super::OnInstructionLoaded_Implementation(Args);
	InitProperties(Args);

	//Prepare Background Actor
	BackgroundImage = UMuthMBPLib::LoadTextureInGame(this, ImageFileName);
	if (!BackgroundImage)
		DestroySelf(); //Image Texture is invalid, destroy;

	AStaticMeshActor* BGActor = Cast<AStaticMeshActor>(GetGlobalObjectData("_MuthM_Background_Actor"));
	if (!BGActor)
	{
		UStaticMesh* Plane = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
		//Spawn Background Actor if there have no actor yet.
		FActorSpawnParameters ASP;
		ASP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		BGActor = GetWorld()->SpawnActor<AStaticMeshActor>(ASP);
		BGActor->SetMobility(EComponentMobility::Movable);
		BGActor->SetActorEnableCollision(false);
		BGActor->SetActorScale3D(FVector(60.f, 33.75f, 1.f));
		BGActor->GetStaticMeshComponent()->SetStaticMesh(Plane);
		BGActor->GetStaticMeshComponent()->SetVisibility(false);
		SetGlobalObjectData("_MuthM_Background_Actor",BGActor);
	}
}

void UBackground::OnInstructionLoaded_Editor_Implementation(FBlueprintJsonObject Args, FEditorExtraInfo EditorExtraInfo)
{
	Super::OnInstructionLoaded_Editor_Implementation(Args,EditorExtraInfo);
	InitProperties(Args);
}

bool UBackground::IsInstructionReady_Implementation(float CurrentTime) const
{
	return (GetTime() - CurrentTime) < 2.f;
}

void UBackground::OnTick_Implementation(float CurrentTime)
{
	if (AnimationProgress < 0.f)
		return; //Animation hasn't begin yet.
	AnimationProgress += (CurrentTime - GetTime()) / AnimationLength;
	if (AnimationProgress > 1.f)
	{
		AnimationProgress = 1.f;
		DestroySelf(); //Animation Finished
	}
	if (CachedDMI)
		CachedDMI->SetScalarParameterValue("AnimationAlpha", AnimationProgress);
}

FBlueprintJsonObject UBackground::GenArgsJsonObject_Implementation()
{
	FBlueprintJsonObject BPJsonObj=Super::GenArgsJsonObject_Implementation();
	BPJsonObj.Object->SetStringField("Operation", Operation);
	BPJsonObj.Object->SetStringField("ImageFileName", ImageFileName);
	BPJsonObj.Object->SetStringField("TargetAnimation", TargetAnimation);
	BPJsonObj.Object->SetNumberField("AnimationLength", (double)AnimationLength);
	return BPJsonObj;
}

#undef LOCTEXT_NAMESPACE