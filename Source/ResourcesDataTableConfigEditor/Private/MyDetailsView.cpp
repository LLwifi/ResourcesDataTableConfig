// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDetailsView.h"

void UMyDetailsView::SetCategoriesToShow(TArray<FName> NewCategoriesToShow)
{
	CategoriesToShow = NewCategoriesToShow;
}

void UMyDetailsView::SetPropertiesToShow(TArray<FName> NewPropertiesToShow)
{
	PropertiesToShow = NewPropertiesToShow;
}

void UMyDetailsView::Refresh()
{
	UObject* Cur = GetObject();
	SetObject(nullptr);
	SetObject(Cur);
}
