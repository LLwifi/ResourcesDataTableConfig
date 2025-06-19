// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ResourcesStructAndEnum.h"
#include "ResourcePropertyGroupEditor.generated.h"

/**
 * 
 */
UCLASS()
class RESOURCESDATATABLECONFIGEDITOR_API UResourcePropertyGroupEditor : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		FName GetRowName();
	virtual FName GetRowName_Implementation();

	UFUNCTION(BlueprintCallable)
		void SaveToDataTable(FResourceTypeGroup PropertyTypeGroup);

	UFUNCTION(BlueprintPure)
		UDataTable* GetResourceTypeGroup();
};
