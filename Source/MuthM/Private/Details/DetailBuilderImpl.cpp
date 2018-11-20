// Copyright (C) 2018 JackMyth. All Rights Reserved.

#include "DetailBuilderImpl.h"

void UDetailBuilderImpl::AddCategory(FDetailCategoryStruct Category)
{
	_DetailCategorylist.FindOrAdd(Category.Title).ItemList.Append(Category.ItemList);
}

UUserWidget* UDetailBuilderImpl::GenDetails()
{
	//UUserWidget* TargetUserWidget=
	return nullptr;
}
