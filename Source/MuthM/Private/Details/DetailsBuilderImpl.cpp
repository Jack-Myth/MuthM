// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailsBuilderImpl.h"
#include "DetailsListBase.h"
#include "UIProvider.h"
#include "HasDetails.h"
#include "DetailInputCustomBase.h"
#include "DetailCategoryBase.h"
#include "DetailStructures.h"
#include "DetailInputStringBase.h"
#include "DetailInputNumberBase.h"

void UDetailsBuilderImpl::AddCategory(FDetailCategoryStruct Category)
{
	for (int i=0;i<_DetailCategorylist.Num();i++)
	{
		if (_DetailCategorylist[i].Title==Category.Title)
		{
			_DetailCategorylist[i].ItemList.Append(Category.ItemList);
			return;
		}
	}
	//If Category hasn't added.
	_DetailCategorylist.Add(Category);
}

void UDetailsBuilderImpl::CollectDetails()
{
	if (!::IsValid(DetailsHolder.GetObject()))
		return;
	_DetailCategorylist.Empty();
	TScriptInterface<IDetailsBuilder> thisBuilder = this;
	IHasDetails::Execute_OnBuildingDetails(DetailsHolder.GetObject(),thisBuilder);
}

UDetailsListBase* UDetailsBuilderImpl::GenDetailsWidget()
{
	auto* TargetUserWidget = Cast<UDetailsListBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get(this)->GetDetailsList(), NAME_None));
	if (!TargetUserWidget)
		return nullptr;
	for (auto it=_DetailCategorylist.CreateIterator();it;++it)
	{
		//Clear Empty Category;
		if (it->ItemList.Num() == 0)
			it.RemoveCurrent();
	}
	for (auto it = _DetailCategorylist.CreateIterator(); it; ++it)
	{
		auto* CategoryWidget = Cast<UDetailCategoryBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get(this)->GetDetailCategory(), NAME_None));
		CategoryWidget->SetHeaderText(it->DisplayTitle);
		for (int i=0;i<it->ItemList.Num();i++)
		{
			switch (it->ItemList[i]->InputType)
			{
				case EDetailInputType::DIT_String:
					{
						TSharedPtr<FDetailItemString> pDetailItemStr = StaticCastSharedPtr<FDetailItemString>(it->ItemList[i]);
						auto* DetailInputStrWidget = Cast<UDetailInputStringBase>(
							UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get(this)->GetDetailInputString(), NAME_None));
						DetailInputStrWidget->ValueInit(pDetailItemStr);
						CategoryWidget->AddDetailItemWidget(DetailInputStrWidget);
					}
					break;
				case EDetailInputType::DIT_Number:
					{
						TSharedPtr<FDetailItemNumber> pDetailItemNumber = StaticCastSharedPtr<FDetailItemNumber>(it->ItemList[i]);
						auto* DetailInputNumberWidget = Cast<UDetailInputNumberBase>(
							UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get(this)->GetDetailInputNumber(), NAME_None));
						DetailInputNumberWidget->ValueInit(pDetailItemNumber);
						CategoryWidget->AddDetailItemWidget(DetailInputNumberWidget);
					}
					break;
				case EDetailInputType::DIT_Custom:
					{
						TSharedPtr<FDetailItemCustom> pDetailItemCustom = StaticCastSharedPtr<FDetailItemCustom>(it->ItemList[i]);
						auto* DetailInputCustomWidget = Cast<UDetailInputCustomBase>(
							UUserWidget::CreateWidgetInstance(*GetWorld(), pDetailItemCustom->CustomWidgetClass, NAME_None));
						DetailInputCustomWidget->ValueInit(pDetailItemCustom);
						CategoryWidget->AddDetailItemWidget(DetailInputCustomWidget);
					}
					break;
			}
		}
		TargetUserWidget->AddCategoryWidget(CategoryWidget);
	}
	return TargetUserWidget;
}

class UWorld* UDetailsBuilderImpl::GetWorld() const
{
	return DetailsHolder.GetObject()->GetWorld();
}

void UDetailsBuilderImpl::SetDetailsHolder(TScriptInterface<IHasDetails> _DetailsHolder)
{
	DetailsHolder = _DetailsHolder;
}

void UDetailsBuilderImpl::AddCategoryBefore(FDetailCategoryStruct Category, FName Before)
{
	for (int i = 0; i < _DetailCategorylist.Num(); i++)
	{
		if (_DetailCategorylist[i].Title == Before)
		{
			_DetailCategorylist.Insert(Category, i);
			return;
		}
	}
	_DetailCategorylist.Add(Category);
}

FDetailCategoryStruct* UDetailsBuilderImpl::FindCategoryByName(FName CategoryName)
{
	return _DetailCategorylist.FindByPredicate([=](const FDetailCategoryStruct& a) {return a.Title == CategoryName; });
}
