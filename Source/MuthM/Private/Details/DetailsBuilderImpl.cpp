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
	_DetailCategorylist.FindOrAdd(Category.Title).ItemList.Append(Category.ItemList);
}

UDetailsListBase* UDetailsBuilderImpl::GenDetails()
{
	auto* TargetUserWidget = Cast<UDetailsListBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get()->GetDetailsList(), "Details"));
	if (!TargetUserWidget)
		return nullptr;
	DetailsHolder->OnBuildingDetails(this);
	for (auto it = _DetailCategorylist.CreateIterator(); it; ++it)
	{
		auto* CategoryWidget = Cast<UDetailCategoryBase>(UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get()->GetDetailCategory(), "DetailCategory"));
		for (int i=0;i<it->Value.ItemList.Num();i++)
		{
			switch (it->Value.ItemList[i]->InputType)
			{
				case EDetailInputType::DIT_String:
					{
						TSharedPtr<FDetailItemString> pDetailItemStr = StaticCastSharedPtr<FDetailItemString>(it->Value.ItemList[i]);
						auto* DetailInputStrWidget = Cast<UDetailInputStringBase>(
							UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get()->GetDetailInputString(), "DetailInputStr"));
						DetailInputStrWidget->ValueInit(pDetailItemStr);
						CategoryWidget->AddDetailItemWidget(DetailInputStrWidget);
					}
					break;
				case EDetailInputType::DIT_Number:
					{
						TSharedPtr<FDetailItemNumber> pDetailItemNumber = StaticCastSharedPtr<FDetailItemNumber>(it->Value.ItemList[i]);
						auto* DetailInputNumberWidget = Cast<UDetailInputNumberBase>(
							UUserWidget::CreateWidgetInstance(*GetWorld(), UUIProvider::Get()->GetDetailInputNumber(), "DetailInputNumber"));
						DetailInputNumberWidget->ValueInit(pDetailItemNumber);
						CategoryWidget->AddDetailItemWidget(DetailInputNumberWidget);
					}
					break;
				case EDetailInputType::DIT_Custom:
					{
						TSharedPtr<FDetailItemCustom> pDetailItemCustom = StaticCastSharedPtr<FDetailItemCustom>(it->Value.ItemList[i]);
						auto* DetailInputCustomWidget = Cast<UDetailInputCustomBase>(
							UUserWidget::CreateWidgetInstance(*GetWorld(), pDetailItemCustom->CustomWidgetClass, "DetailInputCustom"));
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
