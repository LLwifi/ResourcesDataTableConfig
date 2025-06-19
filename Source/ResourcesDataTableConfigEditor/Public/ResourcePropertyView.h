// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include <ResourcesDataTableConfig/Public/ResourcesStructAndEnum.h>
#include "ResourcePropertyView.generated.h"

/**
 * 
 */
UCLASS()
class UResourcePropertyView : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		FName GetRowName();
	virtual FName GetRowName_Implementation();

	UFUNCTION(BlueprintCallable)
		void ClearAllMap();
	
	UFUNCTION(BlueprintCallable)
		void ShowProperty(FResourceTypeGroup PropertyTypeGroup);

	UFUNCTION(BlueprintCallable)
		void SaveToDataTable(FResourceTypeGroup PropertyTypeGroup);

	UFUNCTION(BlueprintPure)
		TMap<EResourceType, UDataTable*> GetTypeMaping();

	UFUNCTION(BlueprintPure)
		UDataTable* GetResourceTypeGroup();
	
public:
	//该模板涉及到了全部类型，同个该值判断哪些变量要展示
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<EResourceType> AllType;

	//属性-----------------------------------------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, bool> Bool;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, int32> Int32;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, float> Float;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FName> Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FString> String;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FText> Text;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FVector> Vector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FRotator> Rotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, TSoftObjectPtr<UTexture2D>> Texture2D;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, TSoftClassPtr<UObject>> Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FResourceProperty_SoundInfo> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformProperty")
		TMap<FString, FTransform> Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BGMProperty")
		TMap<FString, FBGMInfo> BGM;
};
