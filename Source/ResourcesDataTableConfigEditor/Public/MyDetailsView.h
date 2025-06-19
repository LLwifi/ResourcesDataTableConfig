// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DetailsView.h"
#include "MyDetailsView.generated.h"


/**
 * 
 */
UCLASS()
class UMyDetailsView : public UDetailsView
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		void SetCategoriesToShow(TArray<FName> NewCategoriesToShow);
	
	UFUNCTION(BlueprintCallable)
		void SetPropertiesToShow(TArray<FName> NewPropertiesToShow);

	UFUNCTION(BlueprintCallable)
		void Refresh();
	
};
